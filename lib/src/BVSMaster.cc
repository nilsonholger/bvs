#include "BVSMaster.h"

#include<chrono>
#include<dlfcn.h>



BVSModuleMap BVSMaster::modules;



BVSMaster::BVSMaster(BVSConfig& config)
	: flag(BVSSystemFlag::PAUSE)
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
	, round(0)
{

}



void BVSMaster::registerModule(const std::string& identifier, BVSModule* module)
{
	modules[identifier] = std::shared_ptr<BVSModuleData>(new BVSModuleData{identifier, std::string(), module, nullptr, std::thread(), false, BVSModuleFlag::WAIT, BVSStatus::NONE});
}



BVSMaster& BVSMaster::load(const std::string& id, bool asThread)
{
	std::string identifier;
	std::string library;
	std::string options;

	// search for '.' in identifier and separate identifier and options
	// TODO set INPUT/OUTPUT stuff
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



BVSMaster& BVSMaster::control(const BVSSystemFlag controlFlag)
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
	if (controlFlag == BVSSystemFlag::QUIT)
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
		LOG(2, "master controller forking, now running in its own thread!");
		//create thread with this function and this object
		controlThread = std::thread(&BVSMaster::masterController, this, false);
		return *this;
	}

	// wait until all started threads have reached their control loop
	masterCond.notify_one();
	masterCond.wait(masterLock, [&](){ return threadedModules == runningThreads.load(); });
	runningThreads.store(0);

	// main loop, repeat until BVSFlag::QUIT is set
	while (flag != BVSSystemFlag::QUIT)
	{
		// wait until all threads are synchronized
		masterCond.wait_for(masterLock, std::chrono::milliseconds(100), [&](){ return runningThreads.load() == 0; });

		// act on system flag
		switch (flag)
		{
			case BVSSystemFlag::QUIT: break;
			case BVSSystemFlag::PAUSE:
									  // if not running inside own thread, return
									  if (!controlThread.joinable()) return *this;
									  LOG(3, "master pausing...");
									  masterCond.wait(masterLock, [&](){ return flag != BVSSystemFlag::PAUSE; });
									  LOG(3, "master continuing...");
									  break;
			case BVSSystemFlag::RUN:
			case BVSSystemFlag::STEP:
									  LOG(3, "starting next round, notifying threads and executing modules!");
									  LOG(1, "ROUND: " << round++);

									  // set RUN flag for all modules and signal threads
									  for (auto& it: modules)
									  {
										  it.second->flag = BVSModuleFlag::RUN;
										  if (it.second->asThread)
											  runningThreads.fetch_add(1);
									  }
									  threadCond.notify_all();

									  // iterate through modules executed by master
									  for (auto& it: modules)
									  {
										  if (it.second->asThread) continue;
										  moduleController(*(it.second.get()));
									  }

									  if (flag == BVSSystemFlag::STEP) flag = BVSSystemFlag::PAUSE;
									  break;
			case BVSSystemFlag::STEP_BACK: break;
		}
		LOG(3, "waiting for threads to finish!");

		// return if not control thread
		if (!controlThread.joinable() && flag != BVSSystemFlag::RUN) return *this;
	}

	return *this;
}



BVSMaster& BVSMaster::moduleController(BVSModuleData& data)
{
	switch (data.flag)
	{
		case BVSModuleFlag::QUIT:
			data.module->onClose();
			break;
		case BVSModuleFlag::WAIT:
			break;
		case BVSModuleFlag::RUN:
			// call execution functions
			data.module->preExecute();
			data.status = data.module->execute();
			data.module->postExecute();

			// reset module flag
			data.flag = BVSModuleFlag::WAIT;
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
	runningThreads.fetch_add(1);

	while (bool(data->flag))
	{
		// wait for master to announce next round
		LOG(3, data->identifier << " waiting for next round!");
		masterCond.notify_one();
		threadCond.wait(threadLock, [&](){ return data->flag != BVSModuleFlag::WAIT; });

		// call module control
		moduleController(*(data.get()));

		// tell master that thread has finished this round
		runningThreads.fetch_sub(1);
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
			modules[identifier]->flag = BVSModuleFlag::QUIT;
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

