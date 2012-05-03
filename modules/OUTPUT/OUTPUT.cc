#include "OUTPUT.h"



OUTPUT::OUTPUT(BVSConfig& config)
    : BVSModule()
    , identifier("OUTPUT")
    , logger(identifier)
    , config(config)
{

}



OUTPUT::~OUTPUT()
{

}



BVSStatus OUTPUT::onLoad()
{
    LOG(2, "loaded OUTPUT!");

    return BVSStatus::OK;
}



BVSStatus OUTPUT::preExecute()
{

    return BVSStatus::OK;
}



BVSStatus OUTPUT::execute()
{
    LOG(0, "OUTPUT listening!");

    return BVSStatus::OK;
}



BVSStatus OUTPUT::postExecute()
{
    return BVSStatus::OK;
}



BVSStatus OUTPUT::debugDisplay()
{
    return BVSStatus::OK;
}



BVSStatus OUTPUT::onClose()
{
    return BVSStatus::OK;
}



extern "C" {
    // register with framework
    int bvsRegisterModule(BVSConfig& config)
    {
        BVS::registerModule("OUTPUT", new OUTPUT(config));
        return 0;
    }
}
