#include "INPUT.h"



INPUT::INPUT(BVSConfig& config)
    : BVSModule(config)
    , identifier("INPUT")
    , logger(identifier)
    //, config(config)
{

}



INPUT::~INPUT()
{

}



BVSStatus INPUT::onLoad()
{
    LOG(2, "loaded INPUT!");
    LOG(2, config.getValue<int>("BVSLogger.All", 0));

    return BVSStatus::OK;
}



BVSStatus INPUT::preExecute()
{
    return BVSStatus::OK;
}



BVSStatus INPUT::execute()
{
    LOG(0, "INPUT calling!");

    return BVSStatus::OK;
}



BVSStatus INPUT::postExecute()
{
    return BVSStatus::OK;
}



BVSStatus INPUT::debugDisplay()
{
    return BVSStatus::OK;
}



BVSStatus INPUT::onClose()
{
    return BVSStatus::OK;
}



extern "C" {
    // register with framework
    int bvsRegisterModule(BVSConfig& config)
    {
        BVS::registerModule("INPUT", new INPUT(config));
        return 0;
    }
}
