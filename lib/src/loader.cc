#include <dlfcn.h>

#include <regex>
#include <sstream>

#include "loader.h"

using BVS::Loader;
using BVS::ModuleDataMap;
using BVS::ModuleVector;
using BVS::LibHandle;



ModuleDataMap Loader::modules;



ModuleVector* Loader::hotSwapGraveYard = nullptr;



Loader::Loader(const Info& info)
	: logger{"Loader"},
	info(info)
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
		LOG(0, "Duplicate id for module: " << id << std::endl << "If you try to load a module more than once, use unique ids and the id(library).options syntax!");

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
		LOG(0, "Loading function " << function << " from '" << tmpLibrary << "' resulted in: " << dlerr);

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
	std::string connection;
	std::string input;
	std::string targetModule;
	std::string targetOutput;
	std::regex regex("(.+?)\\((.+?)\\.(.+?)\\).*");
	std::smatch match;

	while (!options.empty()) {
		// try to match 'input(targetModule.targetOutput).*'
		std::regex_match(options, match, regex);
		connection = match[0].str().empty() ? options : match[0];
		if (match.size()!=4) LOG(0, "Error matching connector settings: " << connection);
		input = match[1];
		targetModule = match[2];
		targetOutput = match[3];

		// check input connector
		if (module->connectors.find(input)==module->connectors.end())
			LOG(0, "Input not found: " << module->id << "." << connection);
		std::shared_ptr<ConnectorData> in = module->connectors[input];
		if (in->type!=ConnectorType::INPUT) {
			printModuleConnectors(module);
			LOG(0, "Input not found: " << module->id << "." << input);
		}
		if (in->active) LOG(0, "Input already connected: " << module->id << "." << input);

		// check target module and connector
		if (modules.find(targetModule)==modules.end())
			LOG(0, "Module not found: " << targetModule << " in " << module->id << "." << connection);
		if (modules.find(targetModule)->second->connectors.find(targetOutput)==modules.find(targetModule)->second->connectors.end())
			LOG(0, "Output not found: " << targetModule << "." << input);
		std::shared_ptr<ConnectorData> out = modules[targetModule]->connectors[targetOutput];;
		if (out->type!=ConnectorType::OUTPUT) {
			printModuleConnectors(modules.find(targetModule)->second.get());
			LOG(0, "Output not found: " << targetOutput << " in " << module->id << "." << module->options);
		}

		// check type matching
		if (connectorTypeMatching && in->typeIDHash!=out->typeIDHash)
			LOG(0, "Incompatible types for: " << connection << " -> "
					<< in->typeIDName << "(" << in->typeIDHash << ") != "
					<< out->typeIDName << "(" << out->typeIDHash << ")");

		// connect
		in->lock = std::unique_lock<std::mutex>{out->mutex, std::defer_lock};
		in->pointer = out->pointer;
		out->active = true;
		LOG(2, "Connected: " << module->id << "." << input << " <- " << targetModule << "." << targetOutput);

		// remove connection from option string
		options.erase(0, options.find(")")+1);
		if (options[0]=='.') options.erase(0, 1);
	}

	return *this;
}



Loader& Loader::disconnectModule(const std::string& id)
{
	for (auto& disconnect: modules[id]->connectors) {
		if (!disconnect.second->active) continue;
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
					if (onlyConnection && origin) origin->active = false;
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
	if (dlerr) LOG(0, "Loading function bvsHotSwapModule() in '" << modules[id]->library << "' resulted in: " << dlerr);

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

	if (dlib==NULL) LOG(0, "Loading '" << modulePath << "' resulted in: " << dlerror());

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
	if (dlerr) LOG(0, "While closing '" << modulePath << "' following error occured: " << dlerror());
	LOG(3, "Library '" << modulePath << "' unloaded!");

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
		std::string connectors;
		for (auto& it: module->connectors) {
			std::stringstream ss;
			ss << it.second->type;
			connectors += it.second->id + "(" + ss.str() + ") ";
		}
		LOG(0, "Module " << module->id << " defines the following connectors: " << connectors);
	}

	return *this;
}

