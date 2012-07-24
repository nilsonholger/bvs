#include "loader.h"

#include<dlfcn.h>



BVS::ModuleMap BVS::Loader::modules;



BVS::Loader::Loader(Control& control, Config& config)
	: control(control)
	, logger("Loader")
	, config(config)
{

}



void BVS::Loader::registerModule(const std::string& id, Module* module)
{
	modules[id] = std::shared_ptr<ModuleData>(new ModuleData{ \
			id, \
			std::string(), \
			std::string(), \
			module, \
			nullptr, \
			std::thread(), \
			false, \
			ModuleFlag::WAIT, \
			Status::NONE, \
			ConnectorMap()});
}



BVS::Loader& BVS::Loader::load(const std::string& moduleTraits, const bool asThread)
{
	/* algorithm:
	 * SEPARATE id(library).options
	 * CHECK for duplicate ids
	 * LOAD the library and CHECK errors
	 * LINK and execute register function, CHECK errors
	 * SAVE metadata
	 * MOVE connectors to metadata
	 * START (un/)threaded module
	 */
	std::string id;
	std::string library;
	std::string options;

	// search for '.' in id and separate id and options
	size_t separator = moduleTraits.find_first_of('.');
	if (separator!=std::string::npos)
	{
		id = moduleTraits.substr(0, separator);
		options = moduleTraits.substr(separator+1, std::string::npos);
	}
	else
		id = moduleTraits;

	// search for '(' in id and separate if necessary
	separator = id.find_first_of('(');
	if (separator!=std::string::npos)
	{
		library = id.substr(separator+1, std::string::npos);
		library.erase(library.length()-1);
		id = id.erase(separator, std::string::npos);
	}
	else
		library = id;

	// search for duplicate id in modules
	if (modules.find(id)!=modules.end())
	{
		LOG(0, "Duplicate id: " << id);
		LOG(0, "If you try to load a module more than once, use unique ids and the id(library).options syntax!");
		exit(-1);
	}

	// prepare path and load the lib
	std::string modulePath = "./lib" + library + ".so";
	LOG(3, id << " will be loaded from " << modulePath);
	void* dlib = dlopen(modulePath.c_str(), RTLD_NOW);

	// check for errors
	if (dlib == NULL)
	{
		LOG(0, "While loading " << modulePath << ", following error occured: " << dlerror());
		exit(-1);
	}

	// look for bvsRegisterModule in loaded lib, check for errors and execute register function
	typedef void (*bvsRegisterModule_t)(const std::string& id, const Config& config);
	bvsRegisterModule_t bvsRegisterModule;
	*reinterpret_cast<void**>(&bvsRegisterModule)=dlsym(dlib, "bvsRegisterModule");

	// check for errors
	char* dlerr = dlerror();
	if (dlerr)
	{
		LOG(0, "Loading function bvsRegisterModule() in " << modulePath << " resulted in: " << dlerr);
		exit(-1);
	}

	// register
	bvsRegisterModule(id, config);
	LOG(2, id << " loaded and registered!");

	// save handle,library name and option string for later use
	modules[id]->dlib = dlib;
	modules[id]->library = library;
	modules[id]->options = options;

	// move connectors from temporary to metadata
	modules[id]->connectors = std::move(ConnectorDataCollector::connectors);

	// set metadata and start as thread if needed
	if (asThread==true)
	{
		LOG(3, id << " will be started in own thread!");
		modules[id]->asThread = true;
		modules[id]->thread = std::thread(&Control::threadController, &control, modules[id]);
		Control::threadedModules++;
	}
	else
	{
		LOG(3, id << " will be controlled by Control!");
		modules[id]->asThread = false;
	}

	return *this;
}



BVS::Loader& BVS::Loader::unload(const std::string& id, const bool eraseFromMap)
{
	/* algorithm:
	 * CHECK thread, signal exit
	 * DISCONNECT connectors
	 * DELETE module instance and connectors
	 * CHECK library handle
	 * CLOSE library
	 * CHECK errors
	 */
	// wait for thread to join, first check if it is still running
	if (modules[id]->asThread == true)
	{
		if (modules[id]->thread.joinable())
		{
			modules[id]->flag = ModuleFlag::QUIT;
			control.threadCond.notify_all();
			LOG(3, "joining: " << id);
			modules[id]->thread.join();
		}
	}

	// disconnect connectors
	for (auto& it: modules)
	{
		for (auto& con: it.second->connectors)
		{
			if (con.second->type==ConnectorType::INPUT) continue;

			// find and reset all inputs connected to output
			for (auto& mods: modules)
			{
				for (auto& modCon: mods.second->connectors)
				{
					if (con.second->pointer==modCon.second->pointer)
					{
						modCon.second->pointer = nullptr;
						modCon.second->active = false;
					}
				}

			}
		}
	}

	// delete module and connectors
	modules[id]->connectors.clear();
	delete modules[id]->module;
	modules[id]->module = nullptr;

	// close lib and check for errors
	std::string modulePath = "./lib" + modules[id]->library + ".so";
	LOG(3, id << " will be closed using " << modulePath);

	// get handle from internals
	void* dlib = modules[id]->dlib;
	if (dlib==nullptr)
	{
		LOG(0, "Requested module " << id << " not found!");
		exit(-1);
	}

	// close the module
	dlclose(dlib);

	// check for errors
	char* dlerr = dlerror();
	if (dlerr)
	{
		LOG(0, "While closing " << modulePath << " following error occured: " << dlerror());
		exit(-1);
	}
	LOG(2, id << " unloaded and deregistered!");

	if (eraseFromMap)
	{
		modules.erase(id);
		LOG(2, id << " erased from map!");
	}

	return *this;
}



BVS::Loader& BVS::Loader::unloadAll()
{
	for (auto& it: modules)
		unload(it.second->id, false);

	modules.clear();

	return *this;
}



BVS::Loader& BVS::Loader::connectModules()
{
	/* algorithm:
	 * FOR EACH module
	 * DO
	 *		SEPARATE input and remove parsed part
	 *		CHECK input exists
	 *		CHECK input type
	 *		SEPARATE module and output
	 *		CHECK module exists and self reference
	 *		CHECK output exists
	 *		CHECK output type
	 *		CHECK input typeid hash == output typeid hash
	 *		CHECK input already connected
	 *		CONNECT
	 * DONE
	 */

	std::string options;
	std::string selection;
	std::string input;
	std::string module;
	std::string output;
	size_t separator;
	size_t separator2;

	// check options for each module
	for (auto& it: modules)
	{
		options = it.second->options;

		while (!options.empty())
		{
			// get input name and selection
			separator = options.find_first_of('(');
			separator2 = options.find_first_of(')');
			selection = options.substr(0, separator2+1);
			if (separator!=std::string::npos)
			{
				module= options.substr(separator+1, separator2-separator-1);
				input = options.substr(0, separator);
			}
			else
			{
				LOG(0, "no input selection found: " << selection);
				exit(1);
			}

			// remove parsed part
			options.erase(0, separator2+1);
			if (options[0] == '.') options.erase(options.begin());

			// check if desired input exists
			if (it.second->connectors.find(input) == it.second->connectors.end())
			{
				LOG(0, "selected input does not exist: " << selection);
				exit(1);
			}

			// check input type
			if (it.second->connectors[input]->type != ConnectorType::INPUT)
			{
				LOG(0, "selected input is not of input type: " << it.second->id << "." << selection);
				exit(1);
			}

			// search for '.' in selection and separate module and output
			separator = module.find_first_of('.');
			if (separator!=std::string::npos)
			{
				output = module.substr(separator+1, std::string::npos);
				module = module.substr(0, separator);
			}
			else
			{
				LOG(0, "no module output selected: " << selection);
				exit(1);
			}

			// check if desired module exists
			if (modules.find(module) == modules.end())
			{
				LOG(0, "could not find module: " << module);
				exit(1);
			}

			// check for sending data to oneself...
			if (module == it.second->id)
			{
				LOG(0, "can not request data from self: " << selection);
				exit(1);
			}

			// check if desired output exists
			if (modules[module]->connectors.find(output) == modules[module]->connectors.end())
			{
				LOG(0, "selected output does not exist: " << selection);
				exit(1);
			}

			// check output type
			if (modules[module]->connectors[output]->type != ConnectorType::OUTPUT)
			{
				LOG(0, "selected output is not of output type: " << selection);
				exit(1);
			}

			// check input typeid hash == output typeid hash
			if (it.second->connectors[input]->hash != modules[module]->connectors[output]->hash)
			{
				LOG(0, "selected input and output template instantiations are of different type: " << it.second->id << "." << selection);
				exit(1);
			}

			// check if input is already connected
			if (it.second->connectors[input]->active)
			{
				LOG(0, "selected input is alreade connected to another output: " << selection);
				exit(1);
			}

			// connect
			it.second->connectors[input]->pointer = modules[module]->connectors[output]->pointer;
			it.second->connectors[input]->active = true;
			LOG(3, "connected: " << it.second->id << "." << it.second->connectors[input]->id << " <- " << modules[module]->id << "." << modules[module]->connectors[output]->id);
		}
	}



	// do maintenance for load/unload, e.g. remove all connections for unloaded module...
	return *this;
}

