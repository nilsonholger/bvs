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
	modules[identifier] = std::shared_ptr<BVSModuleData>(new BVSModuleData{identifier, std::string(), std::string(), module, nullptr, std::thread(), false, BVSModuleFlag::WAIT, BVSStatus::NONE, std::vector<BVSConnector>()});
}



BVSLoader& BVSLoader::load(const std::string& id, bool asThread)
{
	std::string identifier;
	std::string library;
	std::string options;

	// search for '.' in identifier and separate identifier and options (throwaway, not needed here)
	size_t separator = id.find_first_of('.');
	if (separator!=std::string::npos)
	{
		identifier = id.substr(0, separator);
		options = id.substr(separator+1, std::string::npos);
	}
	else
		identifier = id;

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

	// debug output
	for (auto& it: modules)
	{
		LOG(0, "Module: " << it.second->identifier << " [" << it.second->connectors.size() << "]");
		for (auto& con: it.second->connectors)
		{
			LOG(0, "-> " << it.second->identifier << "." << con.identifier);
		}
	}

	// check options for each module
	for (auto& it: modules)
	{
		// TODO NEXT parse option string
		LOG(0, it.second->identifier << ": " << it.second->options);

		// check if desired input exists

		// check if desired module exists

		// check if desired output exists

		// connect
	}



	// check for sending data to oneself...
	// do maintenance for load/unload, e.g. remove all connections for unloaded module...
	return *this;
}

