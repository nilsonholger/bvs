#include "bvs/logger.h"
#include "logsystem.h"



BVS::Logger::Logger(std::string name, unsigned short verbosity, LogTarget target)
	: verbosity(verbosity)
	, target(target)
	, name(name)
	, logSystem(LogSystem::connectToLogSystem())
{
	logSystem->announce(*this);
}



std::ostream& BVS::Logger::out(const int level)
{
	return logSystem->out(*this, level);
}



void BVS::Logger::endl()
{
	logSystem->endl();
}



std::string BVS::Logger::getName() const
{
	return this->name;
}



BVS::Logger::~Logger()
{

}

