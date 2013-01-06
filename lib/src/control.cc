#include <algorithm>
#include <chrono>

#include "control.h"
#include "bvs/utils.h"

using BVS::Control;
using BVS::SystemFlag;



Control::Control(ModuleDataMap& modules, BVS& bvs, Info& info)
	: modules(modules),
	bvs(bvs),
	info(info),
	logger{"Control"},
	activePools{0},
	pools{},
	flag{SystemFlag::PAUSE},
	barrier{},
	masterLock{barrier.attachParty()},
	controlThread{},
	round{0},
	shutdownRequested{false},
	shutdownRound{0}
{
	pools["master"] = std::make_shared<PoolData>("master", ControlFlag::WAIT);
}



Control& Control::masterController(const bool forkMasterController)
{
	if (forkMasterController)
	{
		LOG(3, "master -> FORKED!");
		controlThread = std::thread{&Control::masterController, this, false};
		return *this;
	}
	else
	{
		nameThisThread("master");

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
				LOG(2, "ROUND: " << round);
				info.round = round++;

				for (auto& module: modules)
				{
					if (module.second->status!=Status::OK)
						checkModuleStatus(module.second);
					module.second->flag = ControlFlag::RUN;
				}
				for (auto& pool: pools) pool.second->flag = ControlFlag::RUN;
				activePools.fetch_add(pools.size()-1); //exclude "master" pool

				barrier.notify();
				for (auto& it: pools["master"]->modules) moduleController(*(it.get()));
				info.poolDurations["master"] =
					std::chrono::duration_cast<std::chrono::milliseconds>
					(std::chrono::high_resolution_clock::now() - timer);

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



Control& Control::sendCommand(const SystemFlag controlFlag)
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



SystemFlag Control::queryActiveFlag()
{
	return flag;
}



Control& Control::startModule(std::string id)
{
	auto data = modules[id];

	if (data->poolName.empty()) data->poolName = "master";

	LOG(3, id << " -> POOL(" << data->poolName << ")");
	if (pools.find(data->poolName)==pools.end())
	{
		pools[data->poolName] = std::make_shared<PoolData>(data->poolName, ControlFlag::WAIT);
		pools[data->poolName]->thread = std::thread{&Control::poolController, this, pools[data->poolName]};
		activePools.fetch_add(1);
		waitUntilInactive(id);
		pools[data->poolName]->modules.push_back(modules[id]);
	}
	else
	{
		pools[data->poolName]->modules.push_back(modules[id]);
	}

	return *this;
}



Control& Control::stopModule(std::string id)
{
	// search for pool
	if (modules.find(id)==modules.end()) return *this;
	auto poolName = modules[id]->poolName;
	if (pools.find(poolName)==pools.end()) return *this;
	auto pool = pools[poolName];

	// stop pool
	auto flag = pool->flag;
	pool->flag = ControlFlag::WAIT;
	waitUntilInactive(id);

	// remove module from pool modules
	auto& poolModules = pools[poolName]->modules;
	if (!poolModules.empty())
		poolModules.erase(std::remove_if (poolModules.begin(), poolModules.end(),
				 [&](std::shared_ptr<ModuleData> data) { return data->id==id; }));

	pool->flag = flag;
	barrier.notify();

	return *this;
}



Control& Control::waitUntilInactive(const std::string& id)
{
	while (isActive(id))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds{100});
		barrier.notify();
	}

	return *this;
}



bool Control::isActive(const std::string& id)
{
	if (!modules[id]->poolName.empty())
	{
		if (modules.find(id)==modules.end()) return false;
		if (pools.find(modules[id]->poolName)==pools.end()) return false;
		if (pools[modules[id]->poolName]->flag!=ControlFlag::WAIT) return true;
		else return false;
	}

	return false;
}



Control& Control::moduleController(ModuleData& data)
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



Control& Control::poolController(std::shared_ptr<PoolData> data)
{
	nameThisThread((data->poolName).c_str());
	LOG(3, "POOL(" << data->poolName << ") STARTED!");
	std::unique_lock<std::mutex> threadLock{barrier.attachParty()};
	std::chrono::time_point<std::chrono::high_resolution_clock> poolTimer =
		std::chrono::high_resolution_clock::now();

	while (bool(data->flag) && !data->modules.empty())
	{
		poolTimer = std::chrono::high_resolution_clock::now();
		for (auto& module: data->modules) moduleController(*(module.get()));

		if (data->flag!=ControlFlag::QUIT) data->flag = ControlFlag::WAIT;
		activePools.fetch_sub(1);
		info.poolDurations[data->poolName] =
			std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::high_resolution_clock::now() - poolTimer);
		LOG(3, "POOL(" << data->poolName << ") WAIT!");
		barrier.enqueue(threadLock, [&](){ return data->flag!=ControlFlag::WAIT; });
	}

	pools.erase(pools.find(data->poolName));

	LOG(3, "POOL(" << data->poolName << ") QUITTING!");
	return *this;
}



Control& Control::checkModuleStatus(std::shared_ptr<ModuleData> data)
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

