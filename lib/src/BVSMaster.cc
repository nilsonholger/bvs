#include "BVSMaster.h"
#include<unistd.h>



BVSModuleMap BVSMaster::modules;



BVSMaster::BVSMaster(BVSConfig& config)
	: flag(BVSFlag::PAUSE)
	, logger("BVSMaster")
	, config(config)
	, runningThreads(0)
	, threadedModules(0)
	, masterMutex()
	, masterLock(masterMutex)
	, masterCond()
	, threadMutex()
	, threadCond()
	, controlThread()
{

}



void BVSMaster::registerModule(const std::string& identifier, BVSModule* module)
{
	modules[identifier] = std::shared_ptr<BVSModuleData>(new BVSModuleData{identifier, std::string(), module, nullptr, std::thread(), false, BVSFlag::PAUSE, BVSStatus::NONE});
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

	// set threading metadata if needed
	if (asThread==true)
	{
		LOG(3, identifier << " will be started in own thread!");
		modules[identifier]->asThread = true;
		modules[identifier]->thread = std::thread(&BVSMaster::threadController, this, modules[identifier]);
		threadedModules++;
	}
	else
	{
		LOG(3, identifier << " will be controlled by BVSMaster!");
		modules[identifier]->asThread = false;

		// call library module load function
		modules[identifier]->module->onLoad();
	}

	return *this;
}



BVSMaster& BVSMaster::control(const BVSFlag controlFlag)
{
	LOG(3, "control() called with flag: " << (int)controlFlag);
	flag = controlFlag;

	// check if controlThread is running and notify it, otherwise call control function each time
	if (controlThread.joinable())
	{
		LOG(3, "control() notifying master!");
		masterCond.notify_one();
	}
	else
	{
		LOG(3, "control() calling master control directly!");
		masterController(false);
	}

	// on quitting, wait for control thread if necessary
	if (controlFlag == BVSFlag::QUIT)
	{
		if (controlThread.joinable())
		{
			LOG(2, "waiting for master control thread to join!");
			controlThread.join();
		}
	}

	return *this;
}



BVSMaster& BVSMaster::masterController(const bool forkMasterController)
{
	if (forkMasterController)
	{
		LOG(2, "forking master controller!");
		//create thread with this function and this object
		controlThread = std::thread(&BVSMaster::masterController, this, false);
		return *this;
	}

	// wait until all started threads have reached their control loop
	masterCond.notify_one();
	masterCond.wait(masterLock, [&](){ return threadedModules == runningThreads; });
	runningThreads = 0;

	// main loop, repeat until BVSFlag::QUIT is set
	while (flag != BVSFlag::QUIT)
	{
		// wait until all threads are synchronized
		masterCond.wait(masterLock, [&](){ return runningThreads == 0; });

		// act on system flag
		switch (flag)
		{
			case BVSFlag::QUIT: break;
			case BVSFlag::PAUSE:
				// if not own running inside own thread, return
				if (!controlThread.joinable())
				{
					LOG(2, "returning from master control!");
					return *this;
				}
				LOG(0, "master pausing...");
				masterCond.wait(masterLock, [&](){ return flag != BVSFlag::PAUSE; });
				LOG(0, "master continuing...");
				break;
			case BVSFlag::RUN:
			case BVSFlag::STEP:
				LOG(2, "starting next round, notifying threads and executing modules!");

				// first run: activate all (threaded) modules
				for (auto& it: modules)
				{
					it.second->flag = flag;
					if (it.second->asThread)
						runningThreads++;
				}
				threadCond.notify_all();

				// second run: iterate through remaining modules
				for (auto& it: modules)
				{
					// skip threaded modules
					if (it.second->asThread)
						continue;
					moduleController(it.second);
				}

				if (flag == BVSFlag::STEP) flag = BVSFlag::PAUSE;
				break;
			case BVSFlag::STEP_BACK: break;
		}

		LOG(3, "waiting for threads to finish!");

		// return if not control thread
		if (!controlThread.joinable() && flag != BVSFlag::RUN)
		{
			LOG(2, "returning from master control!");
			return *this;
		}
	}

	return *this;
}



BVSMaster& BVSMaster::moduleController(std::shared_ptr<BVSModuleData> data)
{
	switch (data->flag)
	{
		case BVSFlag::QUIT:
			data->module->onClose();
			break;
		case BVSFlag::PAUSE:
			break;
		case BVSFlag::RUN:
		case BVSFlag::STEP:
			// reset flag
			data->flag = BVSFlag::PAUSE;

			// call execution functions
			data->module->preExecute();
			data->status = data->module->execute();
			data->module->postExecute();
			break;
		case BVSFlag::STEP_BACK:
			data->flag = BVSFlag::PAUSE;
			break;
	}

	return *this;
}



BVSMaster& BVSMaster::threadController(std::shared_ptr<BVSModuleData> data)
{
	// call library module load function
	data->module->onLoad();

	// acquire lock needed for conditional variable
	std::unique_lock<std::mutex> threadLock(threadMutex);

	// tell system that thread has started
	runningThreads++;

	while (bool(data->flag))
	{
		// wait for master to announce next round
		LOG(3, data->identifier << " waiting for next round!");
		masterCond.notify_one();
		threadCond.wait(threadLock, [&](){ return data->flag != BVSFlag::PAUSE; });

		// call module control
		moduleController(data);

		// tell master that thread has finished this round
		runningThreads--;
		masterCond.notify_one();
	}

	return *this;
}



BVSMaster& BVSMaster::unload(const std::string& identifier, const bool eraseFromMap)
{
	// wait for thread to join, first check if it is still running
	if (modules[identifier]->asThread == true)
	{
		if (modules[identifier]->thread.joinable())
		{
			// TODO signal thread to quit
			modules[identifier]->flag = BVSFlag::QUIT;
			threadCond.notify_all();
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



BVSMaster& BVSMaster::unloadAll()
{
	for (auto it: modules)
		unload(it.second->identifier, false);

	modules.clear();

	return *this;
}
