#include "loader.h"

#include<dlfcn.h>



BVS::ModuleMap BVS::Loader::modules;



BVS::ModuleVector BVS::Loader::masterModules;



BVS::Loader::Loader(Control& control, const Info& info)
	: control(control)
	, logger("Loader")
	, info(info)
{

}



void BVS::Loader::registerModule(const std::string& id, Module* module)
{
	modules[id] = std::shared_ptr<ModuleData>(new ModuleData(
			  id
			, std::string()
			, std::string()
			, module
			, nullptr
			, false
			, ModuleFlag::WAIT
			, Status::NONE
			, ConnectorMap()));
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
		LOG(0, "Duplicate id for module: " << id);
		LOG(0, "If you try to load a module more than once, use unique ids and the id(library).options syntax!");
		exit(-1);
	}

	// prepare path and load the lib
	std::string modulePath = "./lib" + library + ".so";
	LOG(3, "Loading " << id << " from " << modulePath << "!");
	void* dlib = dlopen(modulePath.c_str(), RTLD_NOW);

	// check for errors
	if (dlib == NULL)
	{
		LOG(0, "Loading " << modulePath << ", resulted in: " << dlerror());
		exit(-1);
	}

	// look for bvsRegisterModule in loaded lib, check for errors and execute register function
	typedef void (*bvsRegisterModule_t)(const std::string& id, const Info& info);
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
	bvsRegisterModule(id, info);
	LOG(2, "Loading " << id << " successfull!");

	// save handle,library name and option string for later use
	modules[id]->dlib = dlib;
	modules[id]->library = library;
	modules[id]->options = options;

	// move connectors from temporary to metadata
	modules[id]->connectors = std::move(ConnectorDataCollector::connectors);

	// set metadata and start as thread if needed
	if (asThread==true)
	{
		LOG(3, id << " executed by dedicated thread!");
		modules[id]->asThread = true;
		modules[id]->thread = std::thread(&Control::threadController, &control, modules[id]);
		Control::threadedModules++;
	}
	else
	{
		LOG(3, id << " executed by controller!");
		modules[id]->asThread = false;
		masterModules.push_back(modules[id]);
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
			LOG(3, "Waiting for " << id << " to join!");
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
						modCon.second->mutex = nullptr;
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
	LOG(3, id << " unloading from " << modulePath << "!");

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
	LOG(2, id << " unloaded!");

	if (eraseFromMap)
	{
		modules.erase(id);
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



BVS::Loader& BVS::Loader::connectAllModules(const bool connectorTypeMatching)
{
	for (auto& it: modules) connectModule(it.second->id, connectorTypeMatching);

	return *this;
}



BVS::Loader& BVS::Loader::connectModule(const std::string& id, const bool connectorTypeMatching)
{
	/* algorithm:
	 * WHILE option string not empty
	 *	SEPARATE selection
	 * 	CHECK input and output
	 * 	CHECK input typeid hash == output typeid hash
	 * 	CONNECT
	 * DONE
	 */

	ModuleData* module = modules[id].get();
	std::string options = module->options;
	std::string selection;
	std::string input;
	std::string targetModule;
	std::string targetOutput;
	size_t separator;
	size_t separator2;

	while (!options.empty())
	{
		// get input name and selection
		separator = options.find_first_of('(');
		separator2 = options.find_first_of(')');
		selection = options.substr(0, separator2+1);
		if (separator!=std::string::npos)
		{
			input = options.substr(0, separator);
			targetModule= options.substr(separator+1, separator2-separator-1);
		}
		else
		{
			LOG(0, "No input selection found: " << selection);
			exit(1);
		}

		// remove parsed part
		options.erase(0, separator2+1);
		if (options[0] == '.') options.erase(options.begin());

		// search for '.' in selection and separate module and output
		separator = targetModule.find_first_of('.');
		if (separator!=std::string::npos)
		{
			targetOutput = targetModule.substr(separator+1, std::string::npos);
			targetModule = targetModule.substr(0, separator);
		}
		else
		{
			LOG(0, "No module output selected: " << module->id << "." << selection);
			exit(1);
		}

		// check input and output
		checkModuleInput(module, input);
		checkModuleOutput(module, targetModule, targetOutput);

		// check input typeid hash == output typeid hash
		if (connectorTypeMatching && module->connectors[input]->typeIDHash != modules[targetModule]->connectors[targetOutput]->typeIDHash)
		{
			LOG(0, "Selected input and output connector template instantiations are of different type: "
					<< module->id << "." << selection << " -> "
					<< module->connectors[input]->typeIDName << " != " << modules[targetModule]->connectors[targetOutput]->typeIDName);
			exit(1);
		}

		// connect
		module->connectors[input]->pointer = modules[targetModule]->connectors[targetOutput]->pointer;
		module->connectors[input]->mutex = modules[targetModule]->connectors[targetOutput]->mutex;
		LOG(3, "Connected: " << module->id << "." << module->connectors[input]->id << " <- " << modules[targetModule]->id << "." << modules[targetModule]->connectors[targetOutput]->id);
	}

	return *this;
}



BVS::Loader& BVS::Loader::printModuleConnectors(const ModuleData* module)
{
	if (module->connectors.size()==0)
		LOG(0, "Module " << module->id << " does not define any connector!");
	else
	{
		LOG(0, "Module " << module->id << " defines the following connectors: ");
		for (auto& it: module->connectors) LOG(0, it.second->type << ": " << it.second->id);
	}

	return *this;
}



BVS::Loader& BVS::Loader::checkModuleInput(const ModuleData* module, const std::string& inputName)
{
	auto input = module->connectors.find(inputName);

	// check existence and type
	if (input == module->connectors.end() || input->second->type != ConnectorType::INPUT)
	{
		LOG(0, "Input not found: " << module->id << "." << inputName);
		printModuleConnectors(module);
		exit(1);
	}

	// check if input is already connected
	if (input->second->active)
	{
		LOG(0, "Input already connected: " << module->id << "." << inputName);
		exit(1);
	}

	return *this;
}



BVS::Loader& BVS::Loader::checkModuleOutput(const ModuleData* module, const std::string& targetModule, const std::string& targetOutput)
{
	auto target = modules.find(targetModule);

	// check if desired module exists
	if (target == modules.end())
	{
		LOG(0, "Module not found: " << targetModule << " in " << module->id << "." << module->options);
		exit(1);
	}

	auto output = target->second->connectors.find(targetOutput);

	// check output existence and type
	if (output == target->second->connectors.end() || output->second->type != ConnectorType::OUTPUT)
	{
		LOG(0, "Output not found: " << targetOutput << " in " << module->id << "." << module->options);
		printModuleConnectors(target->second.get());
		exit(1);
	}

	return *this;
}

