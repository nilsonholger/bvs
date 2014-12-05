#include "bvs/logger.h"

#ifdef BVS_LOG_SYSTEM
#include "logsystem.h"
#endif

using BVS::Logger;



Logger::Logger(const std::string& name, unsigned short verbosity, LogTarget target, std::function<void()> errorHandler)
	: name{name}
	, verbosity{verbosity}
	, target{target}
	, errorHandler{errorHandler}
#ifdef BVS_LOG_SYSTEM
	, logSystem{LogSystem::connectToLogSystem()}
#endif
{
#ifdef BVS_LOG_SYSTEM
	logSystem->announce(*this);
#endif
}



std::ostream& Logger::out(const int level)
{
#ifdef BVS_LOG_SYSTEM
	return logSystem->out(*this, level);
#else
	(void) level;
	return std::cout;
#endif
}



void Logger::endl(const int level)
{
#ifdef BVS_LOG_SYSTEM
	logSystem->endl(*this, level);
#endif
}

