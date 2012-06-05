#include "BVS.h"
#include "BVSLogSystem.h"
#include "BVSMaster.h"



std::map <std::string, BVSModuleData*, std::less<std::string>> BVS::bvsModuleMap;



BVS::BVS(int argc, char** argv)
    : config("BVS", argc, argv)
    , logSystem(BVSLogSystem::connectToLogSystem())
    , logger("BVS")
    , master(new BVSMaster(bvsModuleMap, config))
{
    logSystem->updateSettings(config);
    logSystem->updateLoggerLevels(config);
}



BVS& BVS::loadModules()
{
    // get module list from config
    std::vector<std::string> moduleList;
    config.getValue("BVS.modules", moduleList);

    // check length
    if (moduleList.size()==0)
    {
        LOG(1, "no modules specified, nothing to load!");
        return *this;
    }

    // load all selected modules, check for thread selection ('+')
    bool asThread;
    bool moduleThreads = config.getValue<bool>("BVS.moduleThreads", false);
    bool forceModuleThreads = config.getValue<bool>("BVS.forceModuleThreads", false);
    for (auto it : moduleList)
    {
        asThread = false;

        if (it[0]=='+')
        {
            it.erase(0, 1);
            asThread = true;
        }

        if (it[it.length()-1]=='+')
        {
            it.erase(it.length()-1,it.length());
            asThread = true;
        }

        if (forceModuleThreads==true)
        {
            asThread = true;
        }

        if (moduleThreads==false)
        {
            asThread = false;
        }

        loadModule(it, asThread);
    }

    return *this;
}



BVS& BVS::loadModule(const std::string& name, bool asThread)
{
    master->load(name, asThread);

    return *this;
}



BVS& BVS::loadConfigFile(const std::string& configFile)
{
    config.loadConfigFile(configFile);
    logSystem->updateLoggerLevels(config);

    return *this;
}



BVS& BVS::setLogSystemVerbosity(const unsigned short verbosity)
{
    if (logSystem)
    {
        logSystem->setSystemVerbosity(verbosity);
    }

    return *this;
}



BVS& BVS::enableLogFile(const std::string& file, bool append)
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



BVS& BVS::enableLogConsole(const std::ostream& out)
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



void BVS::registerModule(const std::string& identifier, BVSModule* module)
{
    bvsModuleMap[identifier] = new BVSModuleData{identifier, module, nullptr, false, std::thread()};
}



BVS& BVS::run()
{
    master->control();

    return *this;
}



BVS& BVS::quit()
{
    for (auto it: bvsModuleMap)
    {
        LOG(0, "left: " << bvsModuleMap.size());
        LOG(0, "unload: " << it.second->name << " " << it.second->dlib);
        master->unload(it.second->name);
    }

    return *this;
}
