#include "BVSMaster.h"



BVSModuleMap BVSMaster::modules;



BVSMaster::BVSMaster(BVSConfig& config)
	: logger("BVSMaster")
	, config(config)
	, runningThreads(0)
	, threadedModules(0)
	, masterMutex()
	, masterLock(masterMutex)
	, masterCond()
	, state(0)
	, threadMutex()
	//, threadLock(threadMutex)
	, threadCond()
{

}



void BVSMaster::registerModule(const std::string& identifier, BVSModule* module)
{
	modules[identifier] = std::shared_ptr<BVSModuleData>(new BVSModuleData{identifier, std::string(), module, nullptr, std::thread(), false});
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
	LOG(3, identifier << " loaded and registered!");

	// save handle and library name for later use
	modules[identifier]->dlib = dlib;
	modules[identifier]->library = library;

	// call library module load function
	modules[identifier]->module->onLoad();

	// set threading metadata if needed
	if (asThread==true)
	{
		LOG(3, identifier << " will be started in own thread!");
		modules[identifier]->asThread = true;
		modules[identifier]->thread = std::thread(&BVSMaster::moduleController, this, modules[identifier]);
		//runningThreads++;
		threadedModules++;
	}
	else
	{
		LOG(3, identifier << " will be controlled by BVSMaster!");
		modules[identifier]->asThread = false;
	}

	return *this;
}



BVSMaster& BVSMaster::control()
{
	masterCond.wait(masterLock, [&](){ return threadedModules == runningThreads; });
	runningThreads = 0;

	// TODO NEXT create BVS(System)State to activacte while(true) loops
	//while (true)
	{
		masterCond.wait(masterLock, [&](){ return runningThreads == 0; });
		LOG(3, "starting next round!");

		// first run: activate all threaded modules
		for (auto it: modules)
		{
			if (it.second->asThread)
			{
				LOG(3, "activating: " << it.second->identifier);
				runningThreads++;
			}
			//it.second-> unlock mutex
			//TODO per thread state = RUN
		}
		state = 1;
		threadCond.notify_all();

		// second run: iterate through remaining modules
		for (auto it: modules)
		{
			// skip threaded modules
			if (it.second->asThread)
				continue;
			LOG(3, "executing: " << it.second->identifier);
			moduleController(it.second);
		}

		LOG(3, "waiting for threads to finish!");
		masterCond.wait(masterLock, [&](){ return runningThreads == 0; });
	}

	return *this;
}



BVSMaster& BVSMaster::moduleController(std::shared_ptr<BVSModuleData> data)
{
	LOG(3, "Control of " << data->identifier << "!");
	std::unique_lock<std::mutex> threadLock(threadMutex);

	if (data->asThread)
		runningThreads++;

	//while (true)
	{
		if (data->asThread)
		{
			//runningThreads--;
			masterCond.notify_one();
			LOG(3, data->identifier << " waiting for signal from master!");
			threadCond.wait(threadLock, [&](){
					LOG(0, data->identifier << " state: " << state);
					return state != 0; });
			//threadLock.unlock();
		}

		// TODO
		//data->status = 
		LOG(3, data->identifier << " executing...");
		data->module->execute();

		if (data->asThread)
		{
			runningThreads--;
			masterCond.notify_one();
			LOG(3, data->identifier << " DONE!");
		}

		// if not a thread, return control to master
		//if (!data->asThread)
		//	break;
	}
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
		// TODO potential pitfall if threads quits between call to joinable() and join()
		if (modules[identifier]->thread.joinable())
		{
			// TODO signal thread to quit
			LOG(0, "joining: " << identifier);
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
	LOG(3, identifier << " unloaded and deregistered!");

	return *this;
}



BVSMaster& BVSMaster::unloadAll()
{
	for (auto it: modules)
		unload(it.second->identifier);

	modules.clear();

	return *this;
}
