#include "BVSMaster.h"



BVSModuleMap BVSMaster::modules;



BVSMaster::BVSMaster(BVSConfig& config)
	//, controller()
	//, threadMutex()
	: logger("BVSMaster")
	, config(config)
{

}



void BVSMaster::registerModule(const std::string& identifier, BVSModule* module)
{
	modules[identifier] = std::shared_ptr<BVSModuleData>(new BVSModuleData{identifier, module, std::string(), nullptr, false, std::thread()});
}



BVSMaster& BVSMaster::load(const std::string& id, bool asThread)
{
	std::string library;
	std::string identifier = id;

	// search for '.' in identifier and separate if necessary
	size_t separator = identifier.find_first_of('.');
	if (separator!=std::string::npos)
	{
		library = identifier.substr(separator+1, std::string::npos);
		identifier = identifier.erase(separator, std::string::npos);
	}
	else
		library = identifier;

	// search for duplicate identifier in modules
	if (modules.find(identifier)!=modules.end())
	{
		LOG(0, "Duplicate identifier: " << identifier);
		LOG(0, "If you try to load a module more than once, use unique identifiers and the identifier.library syntax!");
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

	// save handle and library name for later use
	modules[identifier]->dlib = dlib;
	modules[identifier]->library = library;

	// call library module load function
	modules[identifier]->module->onLoad();

	// set threading metadata if needed
	if (asThread==true)
	{
		LOG(2, identifier << " now running in own thread!");
		modules[identifier]->asThread = true;
		modules[identifier]->thread = std::thread(&BVSMaster::moduleController, this, modules[identifier]);
	}
	else
	{
		LOG(2, identifier << " controlled by BVSMaster!");
		modules[identifier]->asThread = false;
	}

	return *this;
}



BVSMaster& BVSMaster::control()
{
	// first run: activate all threaded modules
	for (auto it: modules)
	{
		//if (it.second->asThread)
		//it.second-> unlock mutex
	}

	// second run: iterate through remaining modules
	for (auto it: modules)
	{
		// skip threaded modules
		if (it.second->asThread)
			continue;
		moduleController(it.second);
	}

	return *this;
}



BVSMaster& BVSMaster::moduleController(std::shared_ptr<BVSModuleData> data)
{
	LOG(0, "Control of " << data->name << "!");
	//data->module->execute();


	// condition variable
	// BVSModuleStatus array
	// number of threads waiting
	// master: run through all modules not in own thread in specified order
	// threads: execute, wait on condition variable

	//std::thread t1(&BVSMaster::call_from_thread, this, bvsModuleMap[moduleName]);
	//std::cout << "me main" << std::endl;
	//t1.join();
	//std::cout << "me joined" << std::endl;
	/*
	   LOG(0, "me main");

	   for (auto &it : foo)
	   {
	   it.join();
	   LOG(0, "me joined");
	   }
	   LOG(0, "all joined");

	   controller.notify_all();
	   for (auto &it: threadedModules)
	   {
	   (void) it;
	   std::unique_lock<std::mutex> lock(threadMutex);
	   std::thread::id id = it.get_id();
	   LOG(0, "ROUND: " << i << " from: " << id);
	   controller.wait(lock);
	   }
	   */
	return *this;
}



BVSMaster& BVSMaster::unload(const std::string& identifier)
{
	// wait for thread to join
	if (modules[identifier]->asThread == true)
	{
		if (modules[identifier]->thread.joinable())
			// TODO signal thread to quit
			modules[identifier]->thread.join();
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

	return *this;
}



BVSMaster& BVSMaster::unloadAll()
{
	for (auto it: modules)
		unload(it.second->name);

	modules.clear();

	return *this;
}
