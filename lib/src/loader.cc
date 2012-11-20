#include <dlfcn.h>

#include "loader.h"



BVS::ModuleDataMap BVS::Loader::modules;



BVS::ModuleVector* BVS::Loader::hotSwapGraveYard = nullptr;



BVS::Loader::Loader(const Info& info)
	: logger{"Loader"},
	info(info)
{ }



void BVS::Loader::registerModule(const std::string& id, Module* module, bool hotSwap)
{
	if (hotSwap)
	{
#ifdef BVS_MODULE_HOTSWAP
		// NOTE: hotSwapGraveYard will only be initialized when the HotSwap
		// functionality is used. It is intended as a store for unneeded
		// shared_ptr until the process execution ends, but since it is a
		// static pointer it will never be explicitly deleted.
		if (hotSwapGraveYard==nullptr) hotSwapGraveYard = new ModuleVector();
		hotSwapGraveYard->push_back(std::shared_ptr<Module>(module));
		modules[id]->module.swap(hotSwapGraveYard->back());
#endif //BVS_MODULE_HOTSWAP
	}
	else
	{
		modules[id] = std::shared_ptr<ModuleData>{new ModuleData{id, {}, {},
			module, nullptr, false, {}, ControlFlag::WAIT, Status::OK, {}}};
	}
}



BVS::Loader& BVS::Loader::load(const std::string& id, const std::string& library, const std::string& options, const bool asThread, const std::string& poolName)
{
	if (modules.find(id)!=modules.end())
		LOG(0, "Duplicate id for module: " << id << std::endl << "If you try to load a module more than once, use unique ids and the id(library).options syntax!");

	LibHandle dlib = loadLibrary(id, library);

	// execute bvsRegisterModule in loaded lib
	typedef void (*bvsRegisterModule_t)(const std::string& id, const Info& info);
	bvsRegisterModule_t bvsRegisterModule;
	*reinterpret_cast<void**>(&bvsRegisterModule)=dlsym(dlib, "bvsRegisterModule");

	char* dlerr = dlerror();
	if (dlerr)
	{
		LOG(0, "Loading function bvsRegisterModule() in '" << library << "' resulted in: " << dlerr);
		exit(-1);
	}

	bvsRegisterModule(id, info);

	// load library and save handle, library name and option string for later use
	modules[id]->dlib = dlib;
	modules[id]->library = library;
	modules[id]->options = options;

	// get connectors
	modules[id]->connectors = std::move(ConnectorDataCollector::connectors);
	modules[id]->asThread = asThread;
	modules[id]->poolName = poolName;

	LOG(2, "Loading '" << id << "' successfull!");

	return *this;
}



BVS::Loader& BVS::Loader::unload(const std::string& id)
{
	disconnectModule(id);
	modules[id]->connectors.clear();
	modules[id]->module.reset();
	unloadLibrary(id);
	modules.erase(id);

	return *this;
}



BVS::Loader& BVS::Loader::connectAllModules(const bool connectorTypeMatching)
{
	for (auto& it: modules) connectModule(it.second->id, connectorTypeMatching);

	return *this;
}



BVS::Loader& BVS::Loader::connectModule(const std::string& id, const bool connectorTypeMatching)
{
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
		separator = options.find_first_of('(');
		separator2 = options.find_first_of(')');
		selection = options.substr(0, separator2+1);
		if (separator!=std::string::npos)
		{
			input = options.substr(0, separator);
			targetModule = options.substr(separator+1, separator2-separator-1);
		}
		else
		{
			LOG(0, "No input selection found: " << selection);
			exit(1);
		}

		options.erase(0, separator2+1);
		if (options[0] == '.') options.erase(options.begin());

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

		checkModuleInput(module, input);
		checkModuleOutput(module, targetModule, targetOutput);

		if (connectorTypeMatching && module->connectors[input]->typeIDHash != modules[targetModule]->connectors[targetOutput]->typeIDHash)
		{
			LOG(0, "Selected input and output connector template instantiations are of different type: " << module->id << "." << selection << " -> " << module->connectors[input]->typeIDName << " != " << modules[targetModule]->connectors[targetOutput]->typeIDName);
			exit(1);
		}

		module->connectors[input]->pointer = modules[targetModule]->connectors[targetOutput]->pointer;
		module->connectors[input]->mutex = modules[targetModule]->connectors[targetOutput]->mutex;
		LOG(3, "Connected: " << module->id << "." << module->connectors[input]->id << " <- " << modules[targetModule]->id << "." << modules[targetModule]->connectors[targetOutput]->id);
	}

	return *this;
}



BVS::Loader& BVS::Loader::disconnectModule(const std::string& id)
{
	for (auto& connector: modules[id]->connectors)
	{
		if (connector.second->type==ConnectorType::INPUT) continue;

		for (auto& module: modules)
		{
			for (auto& targetConnector: module.second->connectors)
			{
				if (connector.second->pointer==targetConnector.second->pointer)
				{
					targetConnector.second->active = false;
					targetConnector.second->mutex = nullptr;
					targetConnector.second->pointer = nullptr;
				}
			}
		}
	}

	return *this;
}




#ifdef BVS_MODULE_HOTSWAP
BVS::Loader& BVS::Loader::hotSwapModule(const std::string& id)
{
	unloadLibrary(id);
	LibHandle dlib = loadLibrary(id, modules[id]->library);

	typedef void (*bvsHotSwapModule_t)(const std::string& id, BVS::Module* module);
	bvsHotSwapModule_t bvsHotSwapModule;
	*reinterpret_cast<void**>(&bvsHotSwapModule)=dlsym(dlib, "bvsHotSwapModule");

	char* dlerr = dlerror();
	if (dlerr)
	{
		LOG(0, "Loading function bvsHotSwapModule() in '" << modules[id]->library << "' resulted in: " << dlerr);
		exit(-1);
	}

	bvsHotSwapModule(id, modules[id]->module.get());
	modules[id]->dlib = dlib;

	LOG(2, "Hotswapping '" << id << "' successfull!");

	return *this;
}
#endif //BVS_MODULE_HOTSWAP



BVS::LibHandle BVS::Loader::loadLibrary(const std::string& id, const std::string& library)
{
	std::string modulePath = "lib" + library + ".so";
	LibHandle dlib = dlopen(modulePath.c_str(), RTLD_NOW);
	if (dlib!=NULL)
	{
		LOG(3, "Loading '" << id << "' from '" << modulePath << "'!");
	}
	else
	{
#ifdef BVS_OSX_ANOMALIES
		// additional check for 'dylib' (when compiled under OSX as SHARED instead of MODULE)
		modulePath.resize(modulePath.size()-2);
		modulePath += "dylib";
		dlib = dlopen(modulePath.c_str(), RTLD_NOW);
		if (dlib!=NULL) LOG(3, "Loading " << id << " from " << modulePath << "!");
#endif //BVS_OSX_ANOMALIES
	}

	if (dlib==NULL)
	{
		LOG(0, "Loading '" << modulePath << "' resulted in: " << dlerror());
		exit(-1);
	}

	return dlib;
}



BVS::Loader& BVS::Loader::unloadLibrary(const std::string& id)
{
#ifndef BVS_OSX_ANOMALIES
	std::string modulePath = "./lib" + modules[id]->library + ".so";
#else
	std::string modulePath = "./lib" + modules[id]->library + ".(so|dylib)";
#endif //BVS_OSX_ANOMALIES
	LOG(3, "Module '" << id << "' unloading from '" << modulePath << "'!");

	void* dlib = modules[id]->dlib;
	if (dlib==nullptr)
	{
		LOG(0, "Requested module '" << id << "' not found!");
		exit(-1);
	}

	dlclose(dlib);

	char* dlerr = dlerror();
	if (dlerr)
	{
		LOG(0, "While closing '" << modulePath << "' following error occured: " << dlerror());
		exit(-1);
	}
	LOG(2, "Library '" << modulePath << "' unloaded!");

	return *this;
}



BVS::Loader& BVS::Loader::checkModuleInput(const ModuleData* module, const std::string& inputName)
{
	auto input = module->connectors.find(inputName);

	if (input == module->connectors.end() || input->second->type != ConnectorType::INPUT)
	{
		LOG(0, "Input not found: " << module->id << "." << inputName);
		printModuleConnectors(module);
		exit(1);
	}

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
	auto output = target->second->connectors.find(targetOutput);

	if (target == modules.end())
	{
		LOG(0, "Module not found: " << targetModule << " in " << module->id << "." << module->options);
		exit(1);
	}

	if (output == target->second->connectors.end() || output->second->type != ConnectorType::OUTPUT)
	{
		LOG(0, "Output not found: " << targetOutput << " in " << module->id << "." << module->options);
		printModuleConnectors(target->second.get());
		exit(1);
	}

	return *this;
}



BVS::Loader& BVS::Loader::printModuleConnectors(const ModuleData* module)
{
	if (module->connectors.size()==0)
	{
		LOG(0, "Module " << module->id << " does not define any connector!");
	}
	else
	{
		LOG(0, "Module " << module->id << " defines the following connectors: ");
		for (auto& it: module->connectors) LOG(0, it.second->type << ": " << it.second->id);
	}

	return *this;
}
