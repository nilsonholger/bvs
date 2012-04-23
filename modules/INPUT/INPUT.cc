#include "INPUT.h"



INPUT::INPUT(BVSConfig& config)
    : BVSModule()
    , identifier("INPUT")
    , logger(identifier)
    , config(config)
{

}



INPUT::~INPUT()
{

}



void INPUT::onLoad()
{
    LOG(2, "loaded INPUT!");
}



void INPUT::preExecute()
{

}



void INPUT::execute()
{
    LOG(0, "INPUT calling!");
}



void INPUT::postExecute()
{

}



void INPUT::debugDisplay()
{

}



void INPUT::onClose()
{

}



extern "C" {
    // register with framework
    int bvsRegisterModule(BVSConfig& config)
    {
        BVS::registerModule("INPUT", new INPUT(config));
        return 0;
    }
}
