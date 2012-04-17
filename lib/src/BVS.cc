#include "BVS.h"
#include "BVSLogSystem.h"
#include "BVSModuleLoader.h"



std::map <std::string, BVSModule*, std::less<std::string>> bvsModules;



BVS::BVS(int argc, char** argv)
    : logSystem(BVSLogSystem::connectToLogSystem())
    , logger("BVS")
    , config("BVS")
    , loader(new BVSModuleLoader(config))
{
    config.loadCommandLine(argc, argv);
}



void BVS::loadModules()
{
    loader->load("BVSExampleModule");
    bvsModules["BVSExampleModule"]->onLoad();
}



BVS& BVS::setLogSystemVerbosity(unsigned short verbosity)
{
    if (logSystem)
    {
        logSystem->setSystemVerbosity(verbosity);
    }

    return *this;
}



BVS& BVS::enableLogFile(std::string file, bool append)
{
    if (logSystem)
    {
        logSystem->enableLogFile(file, append);
    }

    return *this;
}



BVS& BVS::disableLogFile()
{
    if (logSystem)
    {
        logSystem->disableLogFile();
    }

    return *this;
}



BVS& BVS::enableLogConsole(std::ostream& out)
{
    if (logSystem)
    {
        logSystem->enableLogConsole(out);
    }

    return *this;
}



BVS& BVS::disableLogConsole()
{
    if (logSystem)
    {
        logSystem->disableLogConsole();
    }

    return *this;
}



BVS& BVS::loadConfigFile(std::string configFile)
{
    config.loadConfigFile(configFile);
    return *this;
}
