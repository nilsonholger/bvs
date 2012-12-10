#include <algorithm>
#include <chrono>

#include "control.h"
#include "bvs/archutils.h"



BVS::Control::Control(ModuleDataMap& modules, BVS& bvs, Info& info)
	: modules(modules),
	bvs(bvs),
	info(info),
	logger{"Control"},
	activePools{0},
	masterPoolModules{},
	pools{},
	flag{SystemFlag::PAUSE},
	barrier{},
	masterLock{barrier.attachParty()},
	controlThread{},
	round{0},
	shutdownRequested{false},
	shutdownRound{0}
{ }



BVS::Control& BVS::Control::masterController(const bool forkMasterController)
{
	if (forkMasterController)
	{
		LOG(3, "master -> FORKED!");
		controlThread = std::thread{&Control::masterController, this, false};
		return *this;
	}
	else
	{
		nameThisThread("masterControl");

		// startup sync
		barrier.notify();
		barrier.enqueue(masterLock, [&](){ return activePools.load()==0; });
		activePools.store(0);
	}

	std::chrono::time_point<std::chrono::high_resolution_clock> timer =
		std::chrono::high_resolution_clock::now();

	while (flag!=SystemFlag::QUIT)
	{
		// round sync
		barrier.enqueue(masterLock, [&](){ return activePools.load()==0; });

		info.lastRoundDuration =
			std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::high_resolution_clock::now() - timer);
		timer = std::chrono::high_resolution_clock::now();

		switch (flag)
		{
			case SystemFlag::QUIT: break;
			case SystemFlag::PAUSE:
				if (!controlThread.joinable()) return *this;
				LOG(3, "PAUSE...");
				barrier.enqueue(masterLock, [&](){ return flag!=SystemFlag::PAUSE; });
				timer = std::chrono::high_resolution_clock::now();
				break;
			case SystemFlag::RUN:
			case SystemFlag::STEP:
				LOG(3, "ROUND: " << round);
				info.round = round++;

				for (auto& module: modules)
				{
					if (module.second->status!=Status::OK)
						checkModuleStatus(module.second);
					module.second->flag = ControlFlag::RUN;
				}
				for (auto& pool: pools) pool.second->flag = ControlFlag::RUN;
				activePools.fetch_add(pools.size());

				barrier.notify();
				for (auto& it: masterPoolModules) moduleController(*(it.get()));

				if (flag==SystemFlag::STEP) flag = SystemFlag::PAUSE;
				LOG(3, "WAIT FOR THREADS AND POOLS!");
				break;
			case SystemFlag::STEP_BACK: break;
		}

		if (shutdownRequested && round==shutdownRound) flag = SystemFlag::QUIT;

		if (!controlThread.joinable() && flag!=SystemFlag::RUN) return *this;
	}

	for (auto& pool: pools) pool.second->flag = ControlFlag::QUIT;
	if (shutdownRequested && round==shutdownRound) bvs.shutdownHandler();

	return *this;
}



BVS::Control& BVS::Control::sendCommand(const SystemFlag controlFlag)
{
	LOG(3, "FLAG: " << (int)controlFlag);
	flag = controlFlag;

	if (controlThread.joinable()) barrier.notify();
	else masterController(false);

	if (controlFlag==SystemFlag::QUIT)
	{
		if (controlThread.joinable() && std::this_thread::get_id()!=controlThread.get_id())
		{
			LOG(3, "JOIN MASTER CONTROLLER!");
			controlThread.join();
		}
	}

	return *this;
}



BVS::SystemFlag BVS::Control::queryActiveFlag()
{
	return flag;
}



BVS::Control& BVS::Control::startModule(std::string id)
{
	std::shared_ptr<ModuleData> data = modules[id];

	if (!data->poolName.empty())
	{
		LOG(3, id << " -> POOL(" << data->poolName << ")");
		if (pools.find(data->poolName)==pools.end())
		{
			pools[data->poolName] = std::make_shared<PoolData>
				(data->poolName, ControlFlag::WAIT);
			pools[data->poolName]->modules.push_back(modules[id]);
			pools[data->poolName]->thread = std::thread
				{&Control::poolController, this, pools[data->poolName]};
			activePools.fetch_add(1);
		}
		else
		{
			pools[data->poolName]->modules.push_back(modules[id]);
		}
	}
	else
	{
		LOG(3, id << " -> MASTER");
		masterPoolModules.push_back(modules[id]);
	}

	return *this;
}



BVS::Control& BVS::Control::quitModule(std::string id)
{
	(void) id;
	// TODO has no function now..., check if pool is running instead
	/*if (modules[id]->asThread==true)
	{
		if (modules[id]->thread.joinable())
		{
			modules[id]->flag = ControlFlag::QUIT;
			barrier.notify();
			LOG(3, "Waiting for '" << id << "' to join!");
			modules[id]->thread.join();
		}
	}*/

	return *this;
}



BVS::Control& BVS::Control::purgeData(const std::string& id)
{
	if (!pools.empty())
	{
		std::string pool = modules[id]->poolName;
		if (!pool.empty())
		{
			ModuleDataVector& poolModules = pools[pool]->modules;
			if (!poolModules.empty())
				poolModules.erase(std::remove_if
						(poolModules.begin(), poolModules.end(),
						 [&](std::shared_ptr<ModuleData> data)
						 { return data->id==id; }));
		}
	}

	if (!masterPoolModules.empty())
		masterPoolModules.erase(std::remove_if
				(masterPoolModules.begin(), masterPoolModules.end(),
				 [&](std::shared_ptr<ModuleData> data)
				 { return data->id==id; }));

	return *this;
}



BVS::Control& BVS::Control::waitUntilInactive(const std::string& id)
{
	while (isActive(id))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds{100});
		barrier.notify();
	}

	return *this;
}



bool BVS::Control::isActive(const std::string& id)
{
	if (!modules[id]->poolName.empty())
	{
		//TODO fix these
		//if (pools[modules[id]->poolName]->flag==ControlFlag::WAIT) return false;
		//else return true;
	}

	return false;
}



BVS::Control& BVS::Control::moduleController(ModuleData& data)
{
	std::chrono::time_point<std::chrono::high_resolution_clock> modTimer =
		std::chrono::high_resolution_clock::now();

	switch (data.flag)
	{
		case ControlFlag::QUIT: break;
		case ControlFlag::WAIT: break;
		case ControlFlag::RUN:
			data.status = data.module->execute();
			data.flag = ControlFlag::WAIT;
			break;
	}

	info.moduleDurations[data.id] =
		std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::high_resolution_clock::now() - modTimer);

	return *this;
}



BVS::Control& BVS::Control::poolController(std::shared_ptr<PoolData> data)
{
	nameThisThread(("[P]"+data->poolName).c_str());
	LOG(3, "POOL(" << data->poolName << ") STARTED!");
	std::unique_lock<std::mutex> threadLock{barrier.attachParty()};

	while (bool(data->flag) && !data->modules.empty())
	{
		for (auto& module: data->modules) moduleController(*(module.get()));

		data->flag = ControlFlag::WAIT;
		activePools.fetch_sub(1);
		LOG(3, "POOL(" << data->poolName << ") WAIT!");
		barrier.enqueue(threadLock, [&](){ return data->flag!=ControlFlag::WAIT; });
	}

	pools.erase(pools.find(data->poolName));

	LOG(3, "POOL(" << data->poolName << ") QUITTING!");
	return *this;
}



BVS::Control& BVS::Control::checkModuleStatus(std::shared_ptr<ModuleData> data)
{
	switch (data->status)
	{
		case Status::OK: break;
		case Status::NOINPUT: break;
		case Status::FAIL: break;
		case Status::WAIT: break;
		case Status::DONE:
			bvs.unloadModule(data->id);
			break;
		case Status::SHUTDOWN:
			if (!shutdownRequested)
			{
				LOG(1, "SHUTDOWN REQUEST BY '" << data->id << "', SHUTTING DOWN IN '" << modules.size() << "' ROUNDS!");
				shutdownRequested = true;
				shutdownRound = round + modules.size();
			}
			break;
	}

	return *this;
}

