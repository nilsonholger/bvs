#include "control.h"

#include<chrono>



std::atomic<int> BVS::Control::runningThreads;



int BVS::Control::threadedModules = 0;



BVS::Control::Control()
	: flag(SystemFlag::PAUSE)
	, logger("Control")
	, masterMutex()
	, masterLock(masterMutex)
	, masterCond()
	, threadMutex()
	, threadCond()
	, controlThread()
	, round(0)
{

}



BVS::Control& BVS::Control::masterController(const bool forkMasterController)
{
	if (forkMasterController)
	{
		LOG(2, "master controller forking, now running in its own thread!");
		//create thread with this function and this object
		controlThread = std::thread(&Control::masterController, this, false);
		return *this;
	}

	// wait until all started threads have reached their control loop
	masterCond.notify_one();
	masterCond.wait(masterLock, [&](){ return threadedModules == runningThreads.load(); });
	runningThreads.store(0);

	// main loop, repeat until SystemFlag::QUIT is set
	while (flag != SystemFlag::QUIT)
	{
		// wait until all threads are synchronized
		masterCond.wait_for(masterLock, std::chrono::milliseconds(10), [&](){ return runningThreads.load() == 0; });

		// act on system flag
		switch (flag)
		{
			case SystemFlag::QUIT:
				break;
			case SystemFlag::PAUSE:
				// if not running inside own thread, return
				if (!controlThread.joinable()) return *this;
				LOG(3, "master pausing...");
				masterCond.wait(masterLock, [&](){ return flag != SystemFlag::PAUSE; });
				LOG(3, "master continuing...");
				break;
			case SystemFlag::RUN:
			case SystemFlag::STEP:
				LOG(3, "starting next round, notifying threads and executing modules!");
				LOG(2, "ROUND: " << round++);

				// set RUN flag for all modules and signal threads
				for (auto& it: Loader::modules)
				{
					it.second->flag = ModuleFlag::RUN;
					if (it.second->asThread)
						runningThreads.fetch_add(1);
				}
				threadCond.notify_all();

				// iterate through modules executed by master
				for (auto& it: Loader::modules)
				{
					if (it.second->asThread) continue;
					moduleController(*(it.second.get()));
				}

				if (flag == SystemFlag::STEP) flag = SystemFlag::PAUSE;
				break;
			case SystemFlag::STEP_BACK:
				break;
		}
		LOG(3, "waiting for threads to finish!");

		// return if not control thread
		if (!controlThread.joinable() && flag != SystemFlag::RUN) return *this;
	}

	return *this;
}



BVS::Control& BVS::Control::sendCommand(const SystemFlag controlFlag)
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
	if (controlFlag == SystemFlag::QUIT)
	{
		if (controlThread.joinable())
		{
			LOG(2, "waiting for master control thread to join!");
			controlThread.join();
		}
	}

	return *this;
}



BVS::Control& BVS::Control::moduleController(ModuleData& data)
{
	switch (data.flag)
	{
		case ModuleFlag::QUIT:
			break;
		case ModuleFlag::WAIT:
			break;
		case ModuleFlag::RUN:
			// call execution functions
			data.status = data.module->execute();

			// reset module flag
			data.flag = ModuleFlag::WAIT;
			break;
	}

	return *this;
}



BVS::Control& BVS::Control::threadController(std::shared_ptr<ModuleData> data)
{
	// acquire lock needed for conditional variable
	std::unique_lock<std::mutex> threadLock(threadMutex);

	// tell system that thread has started
	runningThreads.fetch_add(1);

	while (bool(data->flag))
	{
		// wait for master to announce next round
		LOG(3, data->id << " waiting for next round!");
		masterCond.notify_one();
		threadCond.wait(threadLock, [&](){ return data->flag != ModuleFlag::WAIT; });

		// call module control
		moduleController(*(data.get()));

		// tell master that thread has finished this round
		runningThreads.fetch_sub(1);
	}

	return *this;
}
