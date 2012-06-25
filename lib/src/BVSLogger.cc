#include "BVSLogger.h"
#include "BVSLogSystem.h"



BVSLogger::BVSLogger(std::string name, unsigned short verbosity, BVSLogTarget target)
	: verbosity(verbosity)
	, target(target)
	, name(name)
	, logSystem(BVSLogSystem::connectToLogSystem())
{
	logSystem->announce(*this);
}



std::ostream& BVSLogger::out(const int level)
{
	return logSystem->out(*this, level);
}



void BVSLogger::endl()
{
	logSystem->endl();
}



std::string BVSLogger::getName() const
{
	return this->name;
}



BVSLogger::~BVSLogger()
{

}

