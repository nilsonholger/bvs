#include "BVSMaster.h"
#include<unistd.h>


BVSModuleMap BVSMaster::modules;



BVSMaster::BVSMaster(BVSConfig& config)
	: flag(BVSFlag::RUN)
	, logger("BVSMaster")
	, config(config)
	, runningThreads(0)
	, threadedModules(0)
	, masterMutex()
	, masterLock(masterMutex)
	, masterCond()
	, threadMutex()
	, threadCond()
{

}



void BVSMaster::registerModule(const std::string& identifier, BVSModule* module)
{
	modules[identifier] = std::shared_ptr<BVSModuleData>(new BVSModuleData{identifier, std::string(), module, nullptr, std::thread(), false, BVSFlag::NOOP, BVSStatus::NONE});
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
		LOG(2, identifier << " will be started in own thread!");
		modules[identifier]->asThread = true;
		modules[identifier]->thread = std::thread(&BVSMaster::moduleController, this, modules[identifier]);
		//runningThreads++;
		threadedModules++;
	}
	else
	{
		LOG(2, identifier << " will be controlled by BVSMaster!");
		modules[identifier]->asThread = false;
	}

	return *this;
}



BVSMaster& BVSMaster::control()
{
	int round = 0;
	// wait until all started threads have reached their control loop and reset runningThreads
	masterCond.wait(masterLock, [&](){ return threadedModules == runningThreads; });
	runningThreads = 0;

	// main loop, repeat until explicit break is reached
	while (bool(flag))
	{
		// TODO remove and set flag from outside
		if (round > 2)
		{
			LOG(0, "QUIT");
			flag = BVSFlag::QUIT;
		}

		round++;
		// wait until all threads are synchronized
		masterCond.wait(masterLock, [&](){ return runningThreads == 0; });

		// act on system flag
		switch (flag)
		{
			case BVSFlag::QUIT:
				break;
			case BVSFlag::NOOP:
				break;
			case BVSFlag::RUN:
				LOG(3, "starting next round, notifying threads and executing modules!");

				// first run: activate all threaded modules
				for (auto it: modules)
				{
					it.second->flag = flag;
					if (it.second->asThread)
						runningThreads++;
				}
				threadCond.notify_all();

				// second run: iterate through remaining modules
				for (auto it: modules)
				{
					// skip threaded modules
					if (it.second->asThread)
						continue;
					moduleController(it.second);
				}

				LOG(3, "waiting for threads to finish!");
				break;
			case BVSFlag::STEP:
				break;
			case BVSFlag::STEP_BACK:
				break;
		}

		sleep (1);
	}

	return *this;
}



BVSMaster& BVSMaster::moduleController(std::shared_ptr<BVSModuleData> data)
{
	// acquire lock needed for conditional variable
	std::unique_lock<std::mutex> threadLock(threadMutex);

	// if run by thread, tell system that thread has started
	if (data->asThread)
		runningThreads++;

	while (bool(data->flag))
	{
		// wait for master to announce next round
		if (data->asThread)
		{
			masterCond.notify_one();
			threadCond.wait(threadLock, [&](){ return data->flag != BVSFlag::NOOP; });
		}

		switch (data->flag)
		{
			case BVSFlag::QUIT:
				break;
			case BVSFlag::NOOP:
				break;
			case BVSFlag::RUN:
				data->flag = BVSFlag::NOOP;
				LOG(3, data->identifier << " calling execute()!");
				data->status = data->module->execute();
				break;
			case BVSFlag::STEP:
				data->flag = BVSFlag::NOOP;
				break;
			case BVSFlag::STEP_BACK:
				data->flag = BVSFlag::NOOP;
				break;
		}

		// if not a thread, return control to master
		if (!data->asThread)
			break;

		// tell master that thread has finished this round
		if (data->asThread)
		{
			runningThreads--;
			masterCond.notify_one();
		}
	}

	return *this;
}



BVSMaster& BVSMaster::unload(const std::string& identifier)
{
	// wait for thread to join, first check if it is still running
	if (modules[identifier]->asThread == true)
	{
		if (modules[identifier]->thread.joinable())
		{
			// TODO signal thread to quit
			modules[identifier]->flag = BVSFlag::QUIT;
			threadCond.notify_all();
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
