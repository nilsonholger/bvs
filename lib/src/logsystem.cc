#include <algorithm>
#include <iomanip>

#include "logsystem.h"
#include "bvs/traits.h"

using BVS::LogSystem;
using BVS::NullStream;


NullStream LogSystem::nullStream;



std::shared_ptr<LogSystem> LogSystem::instance = nullptr;



std::shared_ptr<LogSystem> LogSystem::connectToLogSystem()
{
	// check if system exists, if not create first
	if (instance==nullptr)
		instance = std::shared_ptr<LogSystem>{new LogSystem()};
	return instance;
}



LogSystem::LogSystem()
	: loggerLevels{}
	, tmpName{}
	, namePadding{0}
	, logColors{}
	, systemVerbosity{3}
	, outMutex{}
	, outCLI{std::clog.rdbuf()}
	, outFile{}
	, outBoth{outCLI, outFile}
{
	// show bools as "true"/"false" instead of "0"/"1"
	outCLI.setf(outCLI.boolalpha);
	outFile.setf(outFile.boolalpha);
	outBoth.setf(outBoth.boolalpha);
}



std::ostream& LogSystem::out(const Logger& logger, int level)
{
	outMutex.lock();

	// convert name to lowercase
	tmpName = logger.name;
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

	// colorize
	if (logColors) {
		*out << "\033[0m";
		if (level==0) *out << "\033[31m";
		else if (level==1) *out << "\033[33m";
	}
	//
	// prepare log output
	*out << "[" << level << "|" << std::setw(namePadding) << std::left << logger.name << "] ";

	// return stream selected by caller given the systems constraints
	return *out;
}



void LogSystem::endl(const Logger& logger, const int level)
{
	outMutex.unlock();
	if (level==0) logger.errorHandler();
}



LogSystem& LogSystem::setSystemVerbosity(int verbosity)
{
	systemVerbosity = verbosity;

	return *this;
}



LogSystem& LogSystem::announce(const Logger& logger)
{
	std::lock_guard<std::mutex> lock(outMutex);

	// update padding size for fancy (aligned) output
	if (logger.name.length() > namePadding) namePadding = logger.name.length();

	// convert name to lowercase
	tmpName = logger.name;
	std::transform(tmpName.begin(), tmpName.end(), tmpName.begin(), ::tolower);

	if (loggerLevels.find(tmpName)==loggerLevels.end())
		loggerLevels[tmpName] = logger.verbosity;

	return *this;
}



LogSystem& LogSystem::enableLogFile(const std::string& file, bool append)
{
	if (outFile.is_open()) outFile.close();

	// check append flag
	if(append)
		outFile.open(file, std::ios_base::app);
	else
		outFile.open(file);

	return *this;
}



LogSystem& LogSystem::disableLogFile()
{
	outFile.close();

	return *this;
}



LogSystem& LogSystem::enableLogConsole(const std::ostream& out)
{
	// set internals to use given stream's buffer
	outCLI.rdbuf(out.rdbuf());

	return *this;
}



LogSystem& LogSystem::disableLogConsole()
{
	// set internals to log to nirvana
	outCLI.rdbuf(nullStream.rdbuf());

	return *this;
}



LogSystem& LogSystem::updateSettings(Config& config)
{
	// disable log system
	if(config.getValue<bool>("BVS.logSystem", bvs_log_system)==false && bvs_log_system) {
		systemVerbosity = 0;
		disableLogConsole();
		disableLogFile();

		return *this;
	}

	// disable console log output
	if(config.getValue<bool>("BVS.logConsole", bvs_log_to_console)==false)
		disableLogConsole();

	// enable log file, append if selected
	std::string configFile = config.getValue<std::string>("BVS.logFile", bvs_log_to_logfile);
	bool append = false;
	if(!configFile.empty()) {
		if (configFile[0]=='+') {
			configFile.erase(0, 1);
			append = true;
		}
		enableLogFile(configFile, append);
	}

	logColors = config.getValue<bool>("BVS.logColors", bvs_log_colors);

	// check log system verbosity
	systemVerbosity = config.getValue<unsigned short>("BVS.logVerbosity", bvs_log_system_verbosity);

	return *this;
}



LogSystem& LogSystem::updateLoggerLevels(Config& config)
{
	// check for LOGLEVEL.* variables and update logger levels
	for (auto& it : config.dumpOptionStore())
	{
		std::string section = "logger.";
		if (it.first.substr(0, section.length())==section)
		{
			loggerLevels[it.first.substr(section.length(), std::string::npos)] = config.getValue<unsigned short>(it.first, bvs_log_client_default_verbosity);
		}
	}

	return *this;
}

