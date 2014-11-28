#include <dlfcn.h>

#include "loader.h"

using BVS::Loader;
using BVS::ModuleDataMap;
using BVS::ModuleVector;
using BVS::LibHandle;



ModuleDataMap Loader::modules;



ModuleVector* Loader::hotSwapGraveYard = nullptr;



Loader::Loader(const Info& info, std::function<void()> errorHandler)
	: logger{"Loader"},
	info(info),
	errorHandler(errorHandler)
{ }



void Loader::registerModule(const std::string& id, Module* module, bool hotSwap)
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
		modules[id] = std::shared_ptr<ModuleData>{new ModuleData{id, {}, {}, {},
			module, nullptr, {}, ControlFlag::WAIT, Status::OK, std::map<std::string, std::shared_ptr<ConnectorData>, std::less<std::string>>{}}};
	}
}



Loader& Loader::load(const std::string& id, const std::string& library, const std::string& configuration, const std::string& options, const std::string& poolName)
{
	if (modules.find(id)!=modules.end())
	{
		LOG(0, "Duplicate id for module: " << id << std::endl << "If you try to load a module more than once, use unique ids and the id(library).options syntax!");
		errorHandler();
	}

	std::string function = "bvsRegisterModule";
	std::string tmpLibrary = library;
#ifdef BVS_STATIC_MODULES
	function += "_" + library;
#ifdef __ANDROID_API__
	tmpLibrary = "bvs_modules";
#else //__ANDROID_API__
	tmpLibrary = "bvs";
#endif //__ANDROID_API__
#endif //BVS_STATIC_MODULES

	LibHandle dlib = loadLibrary(id, tmpLibrary);

	// execute bvsRegisterModule in loaded lib
	typedef void (*bvsRegisterModule_t)(ModuleInfo moduleInfo, const Info& info);
	bvsRegisterModule_t bvsRegisterModule;

	*reinterpret_cast<void**>(&bvsRegisterModule)=dlsym(dlib, function.c_str());

	const char* dlerr = dlerror();
	if (dlerr && bvsRegisterModule==nullptr)
	{
		LOG(0, "Loading function " << function << " from '" << tmpLibrary << "' resulted in: " << dlerr);
		errorHandler();
	}

	ModuleInfo moduleInfo{id, configuration};
	bvsRegisterModule(moduleInfo, info);

	modules[id]->configuration = configuration;
	modules[id]->dlib = dlib;
	modules[id]->library = tmpLibrary;
	modules[id]->options = options;

	// get connectors
	modules[id]->connectors = std::move(ConnectorDataCollector::connectors);
	modules[id]->poolName = poolName;

	LOG(2, "Loading '" << id << "' successfull!");

	return *this;
}



Loader& Loader::unload(const std::string& id)
{
	disconnectModule(id);
	modules[id]->connectors.clear();
	modules[id]->module.reset();
	unloadLibrary(id);
	modules.erase(id);

	LOG(2, "Unloading '" << id << "' successfull!");

	return *this;
}



Loader& Loader::connectAllModules(const bool connectorTypeMatching)
{
	for (auto& it: modules) connectModule(it.second->id, connectorTypeMatching);

	return *this;
}



Loader& Loader::connectModule(const std::string& id, const bool connectorTypeMatching)
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
			errorHandler();
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
			errorHandler();
		}

		checkModuleInput(module, input);
		checkModuleOutput(module, targetModule, targetOutput);

		if (connectorTypeMatching && module->connectors[input]->typeIDHash != modules[targetModule]->connectors[targetOutput]->typeIDHash)
		{
			LOG(0, "Selected input and output connector template instantiations are of different type: " << module->id << "." << selection << " -> "
					<< module->connectors[input]->typeIDName << "(" << module->connectors[input]->typeIDHash << ") != "
					<< modules[targetModule]->connectors[targetOutput]->typeIDName << "(" << modules[targetModule]->connectors[targetOutput]->typeIDHash << ")");
			errorHandler();
		}

		module->connectors[input]->pointer = modules[targetModule]->connectors[targetOutput]->pointer;
		module->connectors[input]->lock = std::unique_lock<std::mutex>{modules[targetModule]->connectors[targetOutput]->mutex, std::defer_lock};
		modules[targetModule]->connectors[targetOutput]->active = true;
		LOG(3, "Connected: " << module->id << "." << module->connectors[input]->id << " <- " << modules[targetModule]->id << "." << modules[targetModule]->connectors[targetOutput]->id);
	}

	return *this;
}



Loader& Loader::disconnectModule(const std::string& id)
{
	for (auto& disconnect: modules[id]->connectors) {
		switch (disconnect.second->type) {
			case ConnectorType::INPUT:
				{
					bool onlyConnection = true;
					std::shared_ptr<ConnectorData> origin;
					for (auto& module: modules) {
						for (auto& test: module.second->connectors) {
							if (test.second->pointer==disconnect.second->pointer) {
								if (test.second->type==ConnectorType::OUTPUT) origin = test.second;
								if (test.second->type==ConnectorType::INPUT && &test!=&disconnect) onlyConnection = false;
							}
						}
					}
					if (onlyConnection) origin->active = false;
					break;
				}
			case ConnectorType::OUTPUT:
				for (auto& module: modules) {
					for (auto& targetConnector: module.second->connectors) {
						if (disconnect.second->pointer==targetConnector.second->pointer) {
							targetConnector.second->active = false;
							targetConnector.second->pointer = nullptr;
						}
					}
				}
				break;
			case ConnectorType::NOOP:
				break;
		}
	}

	return *this;
}




#ifdef BVS_MODULE_HOTSWAP
Loader& Loader::hotSwapModule(const std::string& id)
{
	unloadLibrary(id);
	LibHandle dlib = loadLibrary(id, modules[id]->library);

	typedef void (*bvsHotSwapModule_t)(const std::string& id, Module* module);
	bvsHotSwapModule_t bvsHotSwapModule;
	*reinterpret_cast<void**>(&bvsHotSwapModule)=dlsym(dlib, "bvsHotSwapModule");

	char* dlerr = dlerror();
	if (dlerr)
	{
		LOG(0, "Loading function bvsHotSwapModule() in '" << modules[id]->library << "' resulted in: " << dlerr);
		errorHandler();
	}

	bvsHotSwapModule(id, modules[id]->module.get());
	modules[id]->dlib = dlib;

	LOG(2, "Hotswapping '" << id << "' successfull!");

	return *this;
}
#endif //BVS_MODULE_HOTSWAP



LibHandle Loader::loadLibrary(const std::string& id, const std::string& library)
{
	std::string modulePath;
	if (library == "bvs") modulePath = "libbvs.so";
	else modulePath = "lib" + library + ".so";

	LibHandle dlib;
	if (modulePath == "bvs") dlib = dlopen(NULL, RTLD_NOW);
	else dlib = dlopen(modulePath.c_str(), RTLD_NOW);

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
		errorHandler();
	}

	return dlib;
}



Loader& Loader::unloadLibrary(const std::string& id)
{
#ifndef BVS_OSX_ANOMALIES
	std::string modulePath = "./lib" + modules[id]->library + ".so";
#else
	std::string modulePath = "./lib" + modules[id]->library + ".(so|dylib)";
#endif //BVS_OSX_ANOMALIES
	LOG(3, "Module '" << id << "' unloading from '" << modulePath << "'!");

	void* dlib = modules[id]->dlib;
	if (dlib==nullptr) LOG(0, "Requested module '" << id << "' not found!");

	dlclose(dlib);

	const char* dlerr = dlerror();
	if (dlerr)
	{
		LOG(0, "While closing '" << modulePath << "' following error occured: " << dlerror());
		errorHandler();
	}
	LOG(3, "Library '" << modulePath << "' unloaded!");

	return *this;
}



Loader& Loader::checkModuleInput(const ModuleData* module, const std::string& inputName)
{
	auto input = module->connectors.find(inputName);

	if (input == module->connectors.end() || input->second->type != ConnectorType::INPUT)
	{
		LOG(0, "Input not found: " << module->id << "." << inputName);
		printModuleConnectors(module);
		errorHandler();
	}

	if (input->second->active)
	{
		LOG(0, "Input already connected: " << module->id << "." << inputName);
		errorHandler();
	}

	return *this;
}



Loader& Loader::checkModuleOutput(const ModuleData* module, const std::string& targetModule, const std::string& targetOutput)
{
	auto target = modules.find(targetModule);
	if (target == modules.end())
	{
		LOG(0, "Module not found: " << targetModule << " in " << module->id << "." << module->options);
		errorHandler();
	}

	auto output = target->second->connectors.find(targetOutput);
	if (output == target->second->connectors.end() || output->second->type != ConnectorType::OUTPUT)
	{
		LOG(0, "Output not found: " << targetOutput << " in " << module->id << "." << module->options);
		printModuleConnectors(target->second.get());
		errorHandler();
	}

	return *this;
}



Loader& Loader::printModuleConnectors(const ModuleData* module)
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

