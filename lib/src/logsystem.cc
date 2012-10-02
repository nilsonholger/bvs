#include "logsystem.h"
#include "bvs/traits.h"

#include<algorithm>
#include<iomanip>



BVS::NullStream BVS::LogSystem::nullStream;



std::shared_ptr<BVS::LogSystem> BVS::LogSystem::instance = nullptr;



std::shared_ptr<BVS::LogSystem> BVS::LogSystem::connectToLogSystem()
{
	// check if system exists, if not create first
	if (instance == nullptr)
	{
		instance = std::shared_ptr<LogSystem>(new LogSystem());
	}
	return instance;
}



BVS::LogSystem::LogSystem()
	: loggerLevels()
	, tmpName()
	, namePadding(0)
	, systemVerbosity(3)
	, outMutex()
	, outCLI(std::cout.rdbuf())
	, outFile()
	, outBoth(outCLI, outFile)
{
	// show bools as "true"/"false" instead of "0"/"1"
	outCLI.setf(outCLI.boolalpha);
	outFile.setf(outFile.boolalpha);
	outBoth.setf(outBoth.boolalpha);
}



std::ostream& BVS::LogSystem::out(const Logger& logger, int level)
{
	outMutex.lock();

	// convert name to lowercase
	tmpName = logger.getName();
	std::transform(tmpName.begin(), tmpName.end(), tmpName.begin(), ::tolower);

	// check verbosity of system and logger
	if (level > systemVerbosity) return nullStream;
	if (level > logger.verbosity) return nullStream;
	if (level > loggerLevels[tmpName]) return nullStream;

	// select (enabled/open) output stream according to selected target
	std::ostream* out;
	switch (logger.target)
	{
		case Logger::OFF:
			out = &nullStream;
			break;
		case Logger::TO_CLI:
			if (outCLI.rdbuf() != nullStream.rdbuf()) out = &outCLI;
			else out = &nullStream;
			break;
		case Logger::TO_FILE:
			if (outFile.is_open()) out = &outFile;
			else out = &nullStream;
			break;
		case Logger::TO_CLI_AND_FILE:
			if (outFile.is_open() && outCLI.rdbuf() != nullStream.rdbuf()) out = &outBoth;
			else if (outFile.is_open()) out = &outFile;
			else if (outCLI.rdbuf() != nullStream.rdbuf()) out = &outCLI;
			else out = &nullStream;
			break;
	}

	// prepare log output
	*out << "[" << level << "|" << std::setw(namePadding) << std::left << logger.getName() << "] ";

	// return stream selected by caller given the systems constraints
	return *out;
}



void BVS::LogSystem::endl()
{
	outMutex.unlock();
}



BVS::LogSystem& BVS::LogSystem::setSystemVerbosity(int verbosity)
{
	systemVerbosity = verbosity;

	return *this;
}



BVS::LogSystem& BVS::LogSystem::announce(const Logger& logger)
{
	// update padding size for fancy (aligned) output
	if (logger.getName().length() > namePadding)
	{
		namePadding = logger.getName().length();
	}

	// convert name to lowercase
	tmpName = logger.getName();
	std::transform(tmpName.begin(), tmpName.end(), tmpName.begin(), ::tolower);

	if (loggerLevels.find(tmpName)==loggerLevels.end())
	{
		loggerLevels[tmpName] = logger.verbosity;
	}

	return *this;
}



BVS::LogSystem& BVS::LogSystem::enableLogFile(const std::string& file, bool append)
{
	if (outFile.is_open()) outFile.close();

	// check append flag
	if(append)
	{
		outFile.open(file, std::ios_base::app);
	}
	else
	{
		outFile.open(file);
	}

	return *this;
}



BVS::LogSystem& BVS::LogSystem::disableLogFile()
{
	outFile.close();

	return *this;
}



BVS::LogSystem& BVS::LogSystem::enableLogConsole(const std::ostream& out)
{
	// set internals to use given stream's buffer
	outCLI.rdbuf(out.rdbuf());

	return *this;
}



BVS::LogSystem& BVS::LogSystem::disableLogConsole()
{
	// set internals to log to nirvana
	outCLI.rdbuf(nullStream.rdbuf());

	return *this;
}



BVS::LogSystem& BVS::LogSystem::updateSettings(Config& config)
{
	// disable log system
	if(config.getValue<bool>("BVS.logSystem", bvs_log_system)==false && bvs_log_system)
	{
		systemVerbosity = 0;
		disableLogConsole();
		disableLogFile();

		return *this;
	}

	// disable console log output
	if(config.getValue<bool>("BVS.logConsole", bvs_log_to_console)==false)
	{
		disableLogConsole();
	}

	// enable log file, append if selected
	std::string configFile = config.getValue<std::string>("BVS.logFile", bvs_log_to_logfile);
	bool append = false;
	if(!configFile.empty())
	{
		if (configFile[0]=='+')
		{
			configFile.erase(0, 1);
			append = true;
		}
		enableLogFile(configFile, append);
	}

	// check log system verbosity
	systemVerbosity = config.getValue<unsigned short>("BVS.logVerbosity", bvs_log_system_verbosity);

	return *this;
}



BVS::LogSystem& BVS::LogSystem::updateLoggerLevels(Config& config)
{
	// check for LOGLEVEL.* variables and update logger levels
	for (auto& it : config.dumpOptionStore())
	{
		if (it.first.substr(0, 10)=="logger.")
		{
			loggerLevels[it.first.substr(10, std::string::npos)] = config.getValue<unsigned short>(it.first, bvs_log_client_default_verbosity);
		}
	}

	return *this;
}
