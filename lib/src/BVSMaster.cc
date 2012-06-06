#include "BVSMaster.h"



	BVSMaster::BVSMaster(BVSModuleMap& modules, BVSConfig& config)
: modules(modules)
	//, controller()
	//, threadMutex()
	, logger("BVSMaster")
	 , config(config)
{

}



BVSMaster& BVSMaster::load(const std::string& moduleName, bool asThread)
{
	// prepare path and load the lib
	std::string modulePath = "./lib" + moduleName + ".so";
	LOG(3, moduleName << " will be loaded from " << modulePath);
	void* dlib = dlopen(modulePath.c_str(), RTLD_NOW);

	// check for errors
	if (dlib == NULL)
	{
		LOG(0, "While loading " << modulePath << ", following error occured: " << dlerror());
		exit(-1);
	}

	// look for bvsRegisterModule in loaded lib, check for errors and execute register function
	typedef void (*bvsRegisterModule_t)(BVSConfig& config);
	bvsRegisterModule_t bvsRegisterModule;
	*reinterpret_cast<void**>(&bvsRegisterModule)=dlsym(dlib, "bvsRegisterModule");

	// check for errors
	char* dlerr = dlerror();
	if (dlerr)
	{
		LOG(0, "Loading function bvsRegisterModule() in " << modulePath << " resulted in: " << dlerr);
		exit(-1);
	}
	bvsRegisterModule(config);
	LOG(2, moduleName << " loaded and registered!");

	// get module ID
	// TODO get moduleId from bvsRegisterModule function
	BVSModuleID id = getModuleID(moduleName);

	// deal with an id of 0, there is no such module
	if (id==0)
	{
		LOG(1, moduleName << "not found, cannot unload!");
		return *this;
	}


	// execute module's onLoad function
	modules[id]->module->onLoad();

	// save handle for later use
	modules[id]->dlib = dlib;

	// set threading metadata if needed
	if (asThread==true)
	{
		LOG(2, moduleName << " now running in own thread!");
		modules[id]->asThread = true;
		modules[id]->thread = std::thread(&BVSMaster::control, this, modules[id]);
		//TODO duplicate names resolve to same ID, if 2 threads are loaded, it crashes hard, problem should exist everywhere...
	}
	else
	{
		LOG(2, moduleName << " controlled by BVSMaster!");
		modules[id]->asThread = false;
	}

	return *this;
}



BVSMaster& BVSMaster::unload(const std::string& moduleName)
{
	// get module ID
	BVSModuleID id = getModuleID(moduleName);

	// deal with an id of 0, there is no such module
	if (id==0)
	{
		LOG(1, moduleName << "not found, cannot unload!");
		return *this;
	}

	// wait for thread to join
	if (modules[id]->asThread == true)
	{
		// TODO signal thread to quit
		if (modules[id]->thread.joinable())
			modules[id]->thread.join();
	}

	// close lib and check for errors
	std::string modulePath = "./lib" + moduleName + ".so";
	LOG(3, moduleName << " will be closed using " << modulePath);

	// get handle from internals
	void* dlib = modules[id]->dlib;
	if (dlib==nullptr)
	{
		LOG(0, "Requested module " << moduleName << " not found!");
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
	LOG(2, moduleName << " unloaded and deregistered!");

	return *this;
}



BVSMaster& BVSMaster::control(std::shared_ptr<BVSModuleData> data)
{
	LOG(0, "Control of " << data->name << "!");


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



BVSModuleID BVSMaster::getModuleID(std::string identifier)
{
	for (auto it: modules)
	{
		if (it.second->name == identifier)
			return it.second->id;
	}
	return 0;
}
