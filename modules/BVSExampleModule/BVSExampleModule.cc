#include "BVSExampleModule.h"



BVSExampleModule::BVSExampleModule()
    : BVSModule()
    , identifier("BVSExampleModule")
    , config(identifier)
    , logger(identifier)
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
    int bvsAddModule()
    {
        bvsModules["BVSExampleModule"] = new BVSExampleModule;
        return 0;
    }
}
