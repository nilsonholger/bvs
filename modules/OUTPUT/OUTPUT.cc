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



void OUTPUT::onLoad()
{
    LOG(2, "loaded OUTPUT!");
}



void OUTPUT::preExecute()
{

}



void OUTPUT::execute()
{
    LOG(0, "OUTPUT listening!");
}



void OUTPUT::postExecute()
{

}



void OUTPUT::debugDisplay()
{

}



void OUTPUT::onClose()
{

}



extern "C" {
    // register with framework
    int bvsRegisterModule(BVSConfig& config)
    {
        BVS::registerModule("OUTPUT", new OUTPUT(config));
        return 0;
    }
}
