#include "control.h"

#include<chrono>



std::atomic<int> BVS::Control::runningThreads;



int BVS::Control::threadedModules = 0;



BVS::Control::Control(Info& info)
	: info(info)
	, flag(SystemFlag::PAUSE)
	, logger("Control")
	, masterMutex()
	, masterLock(masterMutex)
	, masterCond()
	, threadMutex()
	, threadCond()
	, controlThread()
	, round(0)
	, timer(std::chrono::high_resolution_clock::now())
	, timer2(std::chrono::high_resolution_clock::now())
{
	runningThreads.store(0);
}



BVS::Control& BVS::Control::masterController(const bool forkMasterController)
{
	if (forkMasterController)
	{
		LOG(2, "Master controller forking, now running in dedicated thread!");
		controlThread = std::thread(&Control::masterController, this, false);
		return *this;
	}

	masterCond.notify_one();
	masterCond.wait(masterLock, [&](){ return threadedModules == runningThreads.load(); });
	runningThreads.store(0);

	while (flag != SystemFlag::QUIT)
	{
		// synchronize
		masterCond.wait(masterLock, [&](){ return runningThreads.load() == 0; });

		switch (flag)
		{
			case SystemFlag::QUIT:
				break;
			case SystemFlag::PAUSE:
				if (!controlThread.joinable()) return *this;
				LOG(3, "Pausing...");
				masterCond.wait(masterLock, [&](){ return flag != SystemFlag::PAUSE; });
				LOG(3, "Continuing...");
				break;
			case SystemFlag::RUN:
			case SystemFlag::STEP:
				info.round = round;
				LOG(3, "Starting next round, notifying threads and executing modules!");
				LOG(2, "ROUND: " << round++);

				for (auto& it: Loader::modules)
				{
					it.second->flag = ModuleFlag::RUN;
					if (it.second->asThread)
						runningThreads.fetch_add(1, std::memory_order_seq_cst);
				}

				timer2 = std::chrono::high_resolution_clock::now();
				info.lastRoundDuration  =
					std::chrono::duration_cast<std::chrono::milliseconds>(timer2 - timer);
				timer = timer2;
				threadCond.notify_all();


				for (auto& it: Loader::masterModules)
				{
					timer2 = std::chrono::high_resolution_clock::now();
					moduleController(*(it.get()));
					info.moduleDurations[it.get()->id] =
						std::chrono::duration_cast<std::chrono::milliseconds>
						(std::chrono::high_resolution_clock::now() - timer2);
				}

				if (flag == SystemFlag::STEP) flag = SystemFlag::PAUSE;
				break;
			case SystemFlag::STEP_BACK:
				break;
		}
		LOG(3, "Waiting for threads to finish!");

		if (!controlThread.joinable() && flag != SystemFlag::RUN) return *this;
	}

	return *this;
}



BVS::Control& BVS::Control::sendCommand(const SystemFlag controlFlag)
{
	LOG(3, "Control() called with flag: " << (int)controlFlag);
	flag = controlFlag;

	// check if controlThread is running and notify it, otherwise call control function each time
	if (controlThread.joinable())
	{
		LOG(3, "Control() notifying master!");
		masterCond.notify_one();
	}
	else
	{
		LOG(3, "Control() calling master control directly!");
		masterController(false);
	}

	if (controlFlag == SystemFlag::QUIT)
	{
		if (controlThread.joinable())
		{
			LOG(2, "Waiting for master control thread to join!");
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
			data.status = data.module->execute();

			data.flag = ModuleFlag::WAIT;
			break;
	}

	return *this;
}



BVS::Control& BVS::Control::threadController(std::shared_ptr<ModuleData> data)
{
	std::unique_lock<std::mutex> threadLock(threadMutex);

	runningThreads.fetch_add(1, std::memory_order_seq_cst);

	while (bool(data->flag))
	{
		LOG(3, data->id << " Waiting for next round!");
		masterCond.notify_one();
		threadCond.wait(threadLock, [&](){ return data->flag != ModuleFlag::WAIT; });

		moduleController(*(data.get()));

		runningThreads.fetch_sub(1, std::memory_order_seq_cst);
		info.moduleDurations[data.get()->id] =
			std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::high_resolution_clock::now() - timer);
	}

	return *this;
}
