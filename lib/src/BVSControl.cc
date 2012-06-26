#include "BVSControl.h"

#include<chrono>



std::atomic<int> BVSControl::runningThreads;



int BVSControl::threadedModules = 0;



BVSControl::BVSControl()
	: flag(BVSSystemFlag::PAUSE)
	, logger("BVSControl")
	, masterMutex()
	, masterLock(masterMutex)
	, masterCond()
	, threadMutex()
	, threadCond()
	, controlThread()
	, round(0)
{

}



BVSControl& BVSControl::sendCommand(const BVSSystemFlag controlFlag)
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



BVSControl& BVSControl::masterController(const bool forkMasterController)
{
	if (forkMasterController)
	{
		LOG(2, "master controller forking, now running in its own thread!");
		//create thread with this function and this object
		controlThread = std::thread(&BVSControl::masterController, this, false);
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
			case BVSSystemFlag::QUIT:
				break;
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
				for (auto& it: BVSLoader::modules)
				{
					it.second->flag = BVSModuleFlag::RUN;
					if (it.second->asThread)
						runningThreads.fetch_add(1);
				}
				threadCond.notify_all();

				// iterate through modules executed by master
				for (auto& it: BVSLoader::modules)
				{
					if (it.second->asThread) continue;
					moduleController(*(it.second.get()));
				}

				if (flag == BVSSystemFlag::STEP) flag = BVSSystemFlag::PAUSE;
				break;
			case BVSSystemFlag::STEP_BACK:
				break;
		}
		LOG(3, "waiting for threads to finish!");

		// return if not control thread
		if (!controlThread.joinable() && flag != BVSSystemFlag::RUN) return *this;
	}

	return *this;
}



BVSControl& BVSControl::moduleController(BVSModuleData& data)
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



BVSControl& BVSControl::threadController(std::shared_ptr<BVSModuleData> data)
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
