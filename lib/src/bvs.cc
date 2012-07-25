#include "bvs/bvs.h"
#include "control.h"
#include "loader.h"
#include "logsystem.h"



BVS::BVS::BVS(int argc, char** argv)
	: config("BVS", argc, argv)
	, logSystem(LogSystem::connectToLogSystem())
	, logger("BVS")
	, control(new Control())
	, loader(new Loader(*control, config))
{
	logSystem->updateSettings(config);
	logSystem->updateLoggerLevels(config);
}



BVS::BVS& BVS::BVS::loadModules()
{
	// get module list and thread settings from config
	std::vector<std::string> moduleList;
	config.getValue("BVS.modules", moduleList);

	// check length
	if (moduleList.size()==0)
	{
		LOG(1, "no modules specified, nothing to load!");
		return *this;
	}

	// load all selected modules
	bool asThread;
	for (auto& it : moduleList)
	{
		asThread = false;

		// check for thread selection ('+' prefix) and system settings
		if (it[0]=='+')
		{
			it.erase(0, 1);
			asThread = true;
		}

		loadModule(it , asThread);
	}

	return *this;
}



BVS::BVS& BVS::BVS::loadModule(const std::string& id, bool asThread)
{
	bool moduleThreads = config.getValue<bool>("BVS.moduleThreads", BVS_MODULE_THREADS);
	bool forceModuleThreads = config.getValue<bool>("BVS.forceModuleThreads", BVS_MODULE_FORCE_THREADS);

	if (forceModuleThreads) asThread = true;

	if (!moduleThreads) asThread = false;

	loader->load(id, asThread);

	return *this;
}



BVS::BVS& BVS::BVS::unloadModule(const std::string& id)
{
	loader->unload(id);

	return *this;
}



BVS::BVS& BVS::BVS::loadConfigFile(const std::string& configFile)
{
	config.loadConfigFile(configFile);
	logSystem->updateSettings(config);
	logSystem->updateLoggerLevels(config);

	return *this;
}



BVS::BVS& BVS::BVS::setLogSystemVerbosity(const unsigned short verbosity)
{
	if (logSystem) logSystem->setSystemVerbosity(verbosity);

	return *this;
}



BVS::BVS& BVS::BVS::enableLogFile(const std::string& file, bool append)
{
	if (logSystem) logSystem->enableLogFile(file, append);

	return *this;
}



BVS::BVS& BVS::BVS::disableLogFile()
{
	if (logSystem) logSystem->disableLogFile();

	return *this;
}



BVS::BVS& BVS::BVS::enableLogConsole(const std::ostream& out)
{
	if (logSystem) logSystem->enableLogConsole(out);

	return *this;
}



BVS::BVS& BVS::BVS::disableLogConsole()
{
	if (logSystem) logSystem->disableLogConsole();

	return *this;
}



BVS::BVS& BVS::BVS::connectModules()
{
	loader->connectModules(config.getValue<bool>("BVS.connectorTypeMatching", BVS_CONNECTOR_TYPE_MATCHING));

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



BVS::BVS& BVS::BVS::quit()
{
	control->sendCommand(SystemFlag::QUIT);
	loader->unloadAll();

	return *this;
}
