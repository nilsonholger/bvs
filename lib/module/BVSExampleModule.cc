#include "BVSExampleModule.h"



BVSExampleModule::BVSExampleModule(BVSConfig& config)
    : BVSModule()
    , identifier("BVSExampleModule")
    , logger(identifier)
    , config(config)
{

}



BVSExampleModule::~BVSExampleModule()
{

}



void BVSExampleModule::onLoad()
{
    LOG(2, "loaded BVSExampleModule!");
}



void BVSExampleModule::preExecute()
{

}



void BVSExampleModule::execute()
{

}



void BVSExampleModule::postExecute()
{

}



void BVSExampleModule::debugDisplay()
{

}



void BVSExampleModule::onClose()
{

}



extern "C" {
    // register with framework
    int bvsRegisterModule(BVSConfig& config)
    {
        BVS::registerModule("BVSExampleModule", new BVSExampleModule(config));
        return 0;
    }
}
