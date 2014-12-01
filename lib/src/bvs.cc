#include "bvs/bvs.h"
#include "control.h"
#include "loader.h"

#ifdef BVS_LOG_SYSTEM
#include "logsystem.h"
#endif



BVS::BVS::BVS(const int argc, const char** argv, std::function<void()> shutdownHandler)
	: config{"bvs", argc, argv},
	shutdownHandler(shutdownHandler),
	info(Info{bvs_version, config, 0, {}, std::map<std::string, std::chrono::duration<unsigned int, std::milli>>{}, std::map<std::string, std::chrono::duration<unsigned int, std::milli>>{}}),
#ifdef BVS_LOG_SYSTEM
	logSystem{LogSystem::connectToLogSystem()},
	logger{"BVS"},
#endif
	loader{new Loader{info}},
	control{new Control{loader->modules, *this, info, config.getValue<bool>("BVS.logStatistics", bvs_log_statistics)}},
	moduleStack{}
{
#ifdef BVS_LOG_SYSTEM
	LogSystem::setErrorHandler(shutdownHandler);
	logSystem->updateSettings(config);
	logSystem->updateLoggerLevels(config);
	LOG(2, "bvs " << info.version);
#endif
}



BVS::BVS::~BVS()
{
	delete control;
	delete loader;
	LOG(2, "bvs " << info.version);
}



BVS::BVS& BVS::BVS::loadModules()
{
	// get module list from config
	std::vector<std::string> moduleList;
	config.getValue("BVS.modules", moduleList);
	std::string poolName;

	// check length
	if (moduleList.size()==0)
	{
		LOG(1, "No modules specified, nothing to load!");
		return *this;
	}

	// load all selected modules
	bool singlePool;
	for (auto& it : moduleList)
	{
		singlePool = false;
		poolName.clear();

		// check for pool selection ('+' prefix or '[...]') and system settings
		if (it[0]=='+')
		{
			it.erase(0, 1);
			if (it[0]=='[') LOG(0, "Cannot start module in thread AND pool: +" << it);
			singlePool = true;
		}
		else if (it[0]=='[')
		{
			size_t pos = it.find_first_of(']');
			poolName = it.substr(1, pos-1);
			it.erase(0, pos+1);
		}

		loadModule(it , singlePool, poolName);
	}

	return *this;
}



BVS::BVS& BVS::BVS::loadModule(const std::string& moduleTraits, bool singlePool, std::string poolName)
{
	std::string id;
	std::string library;
	std::string configuration;
	std::string options;

	// adapt to (forced) module thread/pools settings
	std::string parallelism = config.getValue<std::string>("BVS.parallelism", bvs_parallelism);

	if (parallelism=="NONE") singlePool = false;
	if (parallelism=="FORCE") singlePool = true;
	if (parallelism!="ANY") poolName.clear();

	// separate id, library, configuration and options
	size_t separator = moduleTraits.find_first_of(".()");
	if (separator==std::string::npos) id = moduleTraits;
	else if (moduleTraits.at(separator)=='.')
	{
		id = moduleTraits.substr(0, separator);
		options = moduleTraits.substr(separator+1, std::string::npos);
	}
	else if (moduleTraits.at(separator)=='(')
	{
		size_t dot = moduleTraits.find_first_of('.');
		size_t rp = moduleTraits.find_first_of(')');
		id = moduleTraits.substr(0, separator);
		library = moduleTraits.substr(separator+1, rp-separator-1);
		options = moduleTraits.substr(rp+2<moduleTraits.size()?rp+2:rp+1, std::string::npos);
		if (dot<rp)
		{
			library = moduleTraits.substr(separator+1, dot-separator-1);
			configuration = moduleTraits.substr(dot+1, rp-dot-1);
		}
	}
	if (library.empty()) library = id;
	if (configuration.empty()) configuration = id;
	if (poolName.empty() && singlePool) poolName = id;

	// load
	loader->load(id, library, configuration, options, poolName);
	control->startModule(id);
	moduleStack.push(id);

	return *this;
}



BVS::BVS& BVS::BVS::unloadModules()
{
	while (!moduleStack.empty())
	{
		if(loader->modules.find(moduleStack.top())!=loader->modules.end())
		{
			unloadModule(moduleStack.top());
		}
		moduleStack.pop();
	}

	return *this;
}



BVS::BVS& BVS::BVS::unloadModule(const std::string& id)
{
	SystemFlag state = control->queryActiveFlag();
	if (state!=SystemFlag::QUIT) control->sendCommand(SystemFlag::PAUSE);

	control->stopModule(id);
	loader->unload(id);

	if (state!=SystemFlag::QUIT) control->sendCommand(state);

	return *this;
}



BVS::BVS& BVS::BVS::loadConfigFile(const std::string& configFile)
{
	config.loadConfigFile(configFile);
#ifdef BVS_LOG_SYSTEM
	logSystem->updateSettings(config);
	logSystem->updateLoggerLevels(config);
#endif

	return *this;
}



BVS::BVS& BVS::BVS::setLogSystemVerbosity(const unsigned short verbosity)
{
#ifdef BVS_LOG_SYSTEM
	if (logSystem) logSystem->setSystemVerbosity(verbosity);
#else
	(void) verbosity;
#endif

	return *this;
}



BVS::BVS& BVS::BVS::enableLogFile(const std::string& file, bool append)
{
#ifdef BVS_LOG_SYSTEM
	if (logSystem) logSystem->enableLogFile(file, append);
#else
	(void) file;
	(void) append;
#endif

	return *this;
}



BVS::BVS& BVS::BVS::disableLogFile()
{
#ifdef BVS_LOG_SYSTEM
	if (logSystem) logSystem->disableLogFile();
#endif

	return *this;
}



BVS::BVS& BVS::BVS::enableLogConsole(const std::ostream& out)
{
#ifdef BVS_LOG_SYSTEM
	if (logSystem) logSystem->enableLogConsole(out);
#else
	(void) out;
#endif

	return *this;
}



BVS::BVS& BVS::BVS::disableLogConsole()
{
#ifdef BVS_LOG_SYSTEM
	if (logSystem) logSystem->disableLogConsole();
#endif

	return *this;
}



BVS::BVS& BVS::BVS::connectAllModules()
{
	loader->connectAllModules(config.getValue<bool>("BVS.connectorTypeMatching", bvs_connector_type_matching));

	return *this;
}



BVS::BVS& BVS::BVS::connectModule(const std::string id)
{
	loader->connectModule(id, config.getValue<bool>("BVS.connectorTypeMatching", bvs_connector_type_matching));

	return *this;
}



BVS::BVS& BVS::BVS::start(bool forkMasterController)
{
	control->masterController(forkMasterController);

	return *this;
}



BVS::BVS& BVS::BVS::run()
{
	control->sendCommand(SystemFlag::RUN);

	return *this;
}



BVS::BVS& BVS::BVS::step()
{
	control->sendCommand(SystemFlag::STEP);

	return *this;
}



BVS::BVS& BVS::BVS::pause()
{
	control->sendCommand(SystemFlag::PAUSE);

	return *this;
}



BVS::BVS& BVS::BVS::hotSwap(const std::string& id)
{
#ifdef BVS_MODULE_HOTSWAP
	if (control->modules.find(id)!=control->modules.end())
	{
		SystemFlag state = control->queryActiveFlag();
		control->sendCommand(SystemFlag::PAUSE);
		control->waitUntilInactive(id);

		loader->hotSwapModule(id);
		control->sendCommand(state);
	}
	else
	{
		LOG(0, "'" << id << "' not found!");
	}
#else //BVS_MODULE_HOTSWAP
	LOG(0, "ERROR: HotSwap disabled, could not hotswap: '" << id << "'!");
#endif //BVS_MODULE_HOTSWAP

	return *this;
}



BVS::BVS& BVS::BVS::quit()
{
	control->sendCommand(SystemFlag::QUIT);
	unloadModules();

	return *this;
}

