#include "BVSLogSystem.h"



BVSNullStream BVSLogSystem::nullStream;



std::shared_ptr<BVSLogSystem> BVSLogSystem::instance = nullptr;



std::shared_ptr<BVSLogSystem> BVSLogSystem::connectToLogSystem()
{
    // check if system exists, if not create first
    if (instance == nullptr)
    {
        instance = std::shared_ptr<BVSLogSystem>(new BVSLogSystem());
    }
    return instance;
}



BVSLogSystem::BVSLogSystem()
    : loggerLevels()
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



std::ostream& BVSLogSystem::out(const BVSLogger& logger, int level)
{
    outMutex.lock();

    // check verbosity of system and logger
    if (level > systemVerbosity) return nullStream;
    if (level > logger.verbosity) return nullStream;
    if (level > loggerLevels[logger.getName()]) return nullStream;

    // select (enabled/open) output stream according to selected target
    std::ostream* out;
    switch (logger.target)
    {
        case BVSLogger::OFF:
            out = &nullStream;
            break;
        case BVSLogger::TO_CLI:
            if (outCLI.rdbuf() != nullStream.rdbuf()) out = &outCLI;
            else out = &nullStream;
            break;
        case BVSLogger::TO_FILE:
            if (outFile.is_open()) out = &outFile;
            else out = &nullStream;
            break;
        case BVSLogger::TO_CLI_AND_FILE:
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



void BVSLogSystem::endl()
{
    outMutex.unlock();
}



BVSLogSystem& BVSLogSystem::setSystemVerbosity(int verbosity)
{
    systemVerbosity = verbosity;

    return *this;
}



BVSLogSystem& BVSLogSystem::announce(const BVSLogger& logger)
{
    // update padding size for fancy (aligned) output
    if (logger.getName().length() > namePadding)
    {
        namePadding = logger.getName().length();
    }

    if (loggerLevels.find(logger.getName())==loggerLevels.end())
    {
        loggerLevels[logger.getName()] = logger.verbosity;
    }

    return *this;
}



BVSLogSystem& BVSLogSystem::enableLogFile(const std::string& file, bool append)
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



BVSLogSystem& BVSLogSystem::disableLogFile()
{
    outFile.close();

    return *this;
}



BVSLogSystem& BVSLogSystem::enableLogConsole(const std::ostream& out)
{
    // set internals to use given stream's buffer
    outCLI.rdbuf(out.rdbuf());

    return *this;
}



BVSLogSystem& BVSLogSystem::disableLogConsole()
{
    // set internals to log to nirvana
    outCLI.rdbuf(nullStream.rdbuf());

    return *this;
}



BVSLogSystem& BVSLogSystem::updateSettings(BVSConfig& config)
{
    if(config.getValue<bool>("BVS.logSystem")==false)
    {
        systemVerbosity = 0;
        disableLogConsole();
        disableLogFile();

        return *this;
    }

    if(config.getValue<bool>("BVS.logConsole")==false)
    {
        disableLogConsole();
    }

    std::string configFile = config.getValue<std::string>("BVS.logFile");
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

    return *this;
}



BVSLogSystem& BVSLogSystem::updateLoggerLevels(BVSConfig& config)
{
    // check for LOGLEVEL.* variables and update logger levels
    for (auto it : config.dumpOptionStore())
    {
        if (it.first.substr(0, 10)=="BVSLogger.")
        {
            // check for overall system setting
            if (it.first.substr(10, std::string::npos)=="All")
            {
                systemVerbosity = config.getValue<unsigned short>(it.first);
                continue;
            }

            // set level override from config
            loggerLevels[it.first.substr(10, std::string::npos)] = config.getValue<unsigned short>(it.first);
        }
    }

    return *this;
}
