#include<thread>

#include "BVS.h"
#include "BVSLogSystem.h"
#include "BVSMaster.h"



std::map <std::string, BVSModule*, std::less<std::string>> BVS::bvsModuleMap;



BVS::BVS(int argc, char** argv)
    : config("BVS", argc, argv)
    , logSystem(BVSLogSystem::connectToLogSystem())
    , logger("BVS")
    , master(new BVSMaster(bvsModuleMap, config))
{
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

    // TODO move thread stuff into master
    // TODO determine module in thread (+/-) and use load argument to tell master
    std::vector<std::thread> foo;
    // load all selected modules
    for (auto it : moduleList)
    {
        master->load(it);

        // call the modules setup function
        bvsModuleMap[it]->onLoad();

        foo.push_back(std::thread(&BVSMaster::call_from_thread, master, bvsModuleMap[it]));
    }

    LOG(0, "me main");

    for (auto &it : foo)
    {
        it.join();
        LOG(0, "me joined");
    }
    LOG(0, "all joined");

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
    bvsModuleMap[identifier] = module;
}



BVS& BVS::run()
{
    //while (true)
    {
        //for (auto it: bvsModuleMap)
            //it.second->execute();
    }

    return *this;
}
