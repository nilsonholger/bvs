#include "control.h"

#include<chrono>



std::atomic<int> BVS::Control::runningThreads;



int BVS::Control::threadedModules = 0;



BVS::Control::Control(Info& info)
	: info(info)
	, flag(SystemFlag::PAUSE)
	, logger("Control")
	//, masterMutex()
	, mutex()
	, masterLock(mutex)
	, monitor()
	//, threadMutex()
	//, threadCond()
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
		LOG(3, "master -> FORKED!");
		controlThread = std::thread(&Control::masterController, this, false);
		return *this;
	}

	monitor.notify_all();
	monitor.wait(masterLock, [&](){ return runningThreads.load() == 0; });
	runningThreads.store(0);

	while (flag != SystemFlag::QUIT)
	{
		// synchronize
		monitor.wait(masterLock, [&](){ return runningThreads.load() == 0; });

		switch (flag)
		{
			case SystemFlag::QUIT:
				break;
			case SystemFlag::PAUSE:
				if (!controlThread.joinable()) return *this;
				LOG(3, "PAUSE...");
				monitor.wait(masterLock, [&](){ return flag != SystemFlag::PAUSE; });
				LOG(3, "CONTINUE...");
				break;
			case SystemFlag::RUN:
			case SystemFlag::STEP:
				info.round = round;
				LOG(3, "ANNOUNCE ROUND: " << round++);

				for (auto& it: Loader::modules)
				{
					it.second->flag.store(ModuleFlag::RUN);
					if (it.second->asThread)
						runningThreads.fetch_add(1);
				}

				timer2 = std::chrono::high_resolution_clock::now();
				info.lastRoundDuration  =
					std::chrono::duration_cast<std::chrono::milliseconds>(timer2 - timer);
				timer = timer2;
				monitor.notify_all();


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
		LOG(3, "WAITING FOR MODULES!");

		if (!controlThread.joinable() && flag != SystemFlag::RUN) return *this;
	}

	masterLock.unlock();

	return *this;
}



BVS::Control& BVS::Control::sendCommand(const SystemFlag controlFlag)
{
	LOG(3, "FLAG: " << (int)controlFlag);
	flag = controlFlag;

	// check if controlThread is running and notify it, otherwise call control function each time
	if (controlThread.joinable()) monitor.notify_all();
	else masterController(false);

	if (controlFlag == SystemFlag::QUIT)
	{
		if (controlThread.joinable())
		{
			LOG(3, "JOIN MASTER CONTROLLER!");
			controlThread.join();
		}
	}

	return *this;
}



BVS::Control& BVS::Control::moduleController(ModuleData& data)
{
	switch (data.flag.load())
	{
		case ModuleFlag::QUIT:
			break;
		case ModuleFlag::WAIT:
			break;
		case ModuleFlag::RUN:
			data.status = data.module->execute();

			data.flag.store(ModuleFlag::WAIT);
			break;
	}

	return *this;
}



BVS::Control& BVS::Control::threadController(std::shared_ptr<ModuleData> data)
{
	std::unique_lock<std::mutex> threadLock(mutex);

	while (data->flag.load()!=ModuleFlag::QUIT)
	{
		runningThreads.fetch_sub(1);
		monitor.notify_all();
		LOG(3, data->id << " -> WAIT!");
		monitor.wait(threadLock, [&](){ return data->flag.load() != ModuleFlag::WAIT; });

		moduleController(*(data.get()));

		info.moduleDurations[data.get()->id] =
			std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::high_resolution_clock::now() - timer);
	}

	return *this;
}
