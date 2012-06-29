#include "BVSLoader.h"

#include<dlfcn.h>



BVSModuleMap BVSLoader::modules;



BVSLoader::BVSLoader(BVSControl& control, BVSConfig& config)
	: control(control)
	, logger("BVSLoader")
	, config(config)
{

}



void BVSLoader::registerModule(const std::string& identifier, BVSModule* module)
{
	modules[identifier] = std::shared_ptr<BVSModuleData>(new BVSModuleData{identifier, std::string(), std::string(), module, nullptr, std::thread(), false, BVSModuleFlag::WAIT, BVSStatus::NONE, BVSConnectorMap()});
}



BVSLoader& BVSLoader::load(const std::string& moduleTraits, const bool asThread)
{
	/* algorithm:
	 * SEPARATE identifier(library).options
	 * CHECK for duplicate identifiers
	 * LOAD the library and CHECK errors
	 * LINK and execute register function, CHECK errors
	 * SAVE metadata
	 * MOVE connectors to metadata
	 * START (un/)threaded module
	 */
	std::string identifier;
	std::string library;
	std::string options;

	// search for '.' in identifier and separate identifier and options (throwaway, not needed here)
	size_t separator = moduleTraits.find_first_of('.');
	if (separator!=std::string::npos)
	{
		identifier = moduleTraits.substr(0, separator);
		options = moduleTraits.substr(separator+1, std::string::npos);
	}
	else
		identifier = moduleTraits;

	// search for '(' in identifier and separate if necessary
	separator = identifier.find_first_of('(');
	if (separator!=std::string::npos)
	{
		library = identifier.substr(separator+1, std::string::npos);
		library.erase(library.length()-1);
		identifier = identifier.erase(separator, std::string::npos);
	}
	else
		library = identifier;

	// search for duplicate identifier in modules
	if (modules.find(identifier)!=modules.end())
	{
		LOG(0, "Duplicate identifier: " << identifier);
		LOG(0, "If you try to load a module more than once, use unique identifiers and the identifier(library).options syntax!");
		exit(-1);
	}

	// prepare path and load the lib
	std::string modulePath = "./lib" + library + ".so";
	LOG(3, identifier << " will be loaded from " << modulePath);
	void* dlib = dlopen(modulePath.c_str(), RTLD_NOW);

	// check for errors
	if (dlib == NULL)
	{
		LOG(0, "While loading " << modulePath << ", following error occured: " << dlerror());
		exit(-1);
	}

	// look for bvsRegisterModule in loaded lib, check for errors and execute register function
	typedef void (*bvsRegisterModule_t)(const std::string& identifier, const BVSConfig& config);
	bvsRegisterModule_t bvsRegisterModule;
	*reinterpret_cast<void**>(&bvsRegisterModule)=dlsym(dlib, "bvsRegisterModule");

	// check for errors
	char* dlerr = dlerror();
	if (dlerr)
	{
		LOG(0, "Loading function bvsRegisterModule() in " << modulePath << " resulted in: " << dlerr);
		exit(-1);
	}
	bvsRegisterModule(identifier, config);
	LOG(2, identifier << " loaded and registered!");

	// save handle,library name and option string for later use
	modules[identifier]->dlib = dlib;
	modules[identifier]->library = library;
	modules[identifier]->options = options;

	// move connectors from temporary to metadata
	modules[identifier]->connectors = std::move(BVSConnector::connectors);

	// set metadata and start as thread if needed
	if (asThread==true)
	{
		LOG(3, identifier << " will be started in own thread!");
		modules[identifier]->asThread = true;
		modules[identifier]->thread = std::thread(&BVSControl::threadController, &control, modules[identifier]);
		BVSControl::threadedModules++;
	}
	else
	{
		LOG(3, identifier << " will be controlled by BVSControl!");
		modules[identifier]->asThread = false;

		// call library module load function
		modules[identifier]->module->onLoad();
	}

	return *this;
}



BVSLoader& BVSLoader::unload(const std::string& identifier, const bool eraseFromMap)
{
	/* algorithm:
	 * CHECK thread, signal exit
	 * CHECK library handle
	 * CLOSE library
	 * CHECK errors
	 */
	// wait for thread to join, first check if it is still running
	if (modules[identifier]->asThread == true)
	{
		if (modules[identifier]->thread.joinable())
		{
			modules[identifier]->flag = BVSModuleFlag::QUIT;
			control.threadCond.notify_all();
			LOG(3, "joining: " << identifier);
			modules[identifier]->thread.join();
		}
	}

	// close lib and check for errors
	std::string modulePath = "./lib" + modules[identifier]->library + ".so";
	LOG(3, identifier << " will be closed using " << modulePath);

	// get handle from internals
	void* dlib = modules[identifier]->dlib;
	if (dlib==nullptr)
	{
		LOG(0, "Requested module " << identifier << " not found!");
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
	LOG(2, identifier << " unloaded and deregistered!");

	if (eraseFromMap)
	{
		modules.erase(identifier);
		LOG(2, identifier << " erased from map!");
	}

	return *this;
}



BVSLoader& BVSLoader::unloadAll()
{
	for (auto it: modules)
		unload(it.second->identifier, false);

	modules.clear();

	return *this;
}



BVSLoader& BVSLoader::connectModules()
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
	 *		CONNECT
	 * DONE
	 */

	// debug output
	/*for (auto& it: modules)
	{
		LOG(0, "Module: " << it.second->identifier << " [" << it.second->connectors.size() << "]");
		for (auto& con: it.second->connectors)
		{
			LOG(0, "-> " << it.second->identifier << "." << con.identifier);
		}
	}*/

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
			selection = options.substr(0, separator2);
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
			if (it.second->connectors[input]->type != BVSConnectorType::IN)
			{
				LOG(0, "selected input is not of input type: " << selection);
				exit(1);
			}

			// search for '.' in selection and separate module and output
			separator = module.find_first_of('.');
			if (separator!=std::string::npos)
			{
				output = module.substr(separator+1, std::string::npos);
				module = module.substr(0, separator);
				LOG(3, it.second->identifier << ": " << input << "(" << module << "." << output << ")");
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
			if (module == it.second->identifier)
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
			if (modules[module]->connectors[output]->type != BVSConnectorType::OUT)
			{
				LOG(0, "selected output is not of output type: " << selection);
				exit(1);
			}

			// check if input is already connected...

			// check if input data type equals output data type

			// connect
			it.second->connectors[input]->data = modules[module]->connectors[output]->data;
		}
	}



	// do maintenance for load/unload, e.g. remove all connections for unloaded module...
	return *this;
}

