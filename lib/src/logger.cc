#include "bvs/logger.h"

#ifdef BVS_LOG_SYSTEM
#include "logsystem.h"
#endif



BVS::Logger::Logger(const std::string& name, unsigned short verbosity, LogTarget target)
	: name{name},
	verbosity{verbosity},
	target{target}
#ifdef BVS_LOG_SYSTEM
	, logSystem{LogSystem::connectToLogSystem()}
#endif
{
#ifdef BVS_LOG_SYSTEM
	logSystem->announce(*this);
#endif
}



std::ostream& BVS::Logger::out(const int level)
{
#ifdef BVS_LOG_SYSTEM
	return logSystem->out(*this, level);
#else
	return std::clog;
#endif
}



BVS::Logger::~Logger()
{

}

