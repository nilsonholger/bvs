#include "BVSLogger.h"
#include "BVSLogSystem.h"



BVSLogger::BVSLogger(std::string name, unsigned short verbosity, BVSLogTarget target)
    : verbosity(verbosity)
    , target(target)
    , name(name)
    , logSystem(BVSLogSystem::connectToLogSystem())
{
    // announce logging instance to system, this updates the name padding
    logSystem->announce(*this);
}



std::ostream& BVSLogger::out(const int level)
{
    // forward the request to the logging system
    return logSystem->out(*this, level);
}



std::string BVSLogger::getName() const
{
    return this->name;
}



BVSLogger::~BVSLogger()
{

}

