#include "BVS.h"
#include "BVSLogSystem.h"
#include "BVSModuleLoader.h"



std::map <std::string, BVSModule*, std::less<std::string>> BVS::bvsModuleMap;



BVS::BVS(int argc, char** argv)
    : logSystem(BVSLogSystem::connectToLogSystem())
    , logger("BVS")
    , config("BVS")
    , loader(new BVSModuleLoader(bvsModuleMap, config))
{
    config.loadCommandLine(argc, argv);
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

    // load all selected modules
    for (auto it : moduleList)
    {
        loader->load(it);

        // call the modules setup function
        bvsModuleMap[it]->onLoad();
    }

    return *this;
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



void BVS::registerModule(std::string identifier, BVSModule* module)
{
    bvsModuleMap[identifier] = module;
}
