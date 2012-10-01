#include "bvs/archutils.h"
#include <iostream>



int BVS::nameThisThread(const char* threadName)
{
#if (defined __unix__ && defined BVS_THREAD_NAMES)
	prctl(PR_SET_NAME, threadName);
	if (errno)
	{
		std::cerr << "[ERROR|BVS] unable to set thread name, error: " << errno << std::endl;
		std::cerr << "[ERROR|BVS] Please disable BVS_THREAD_NAMES in your cmake settings!" << std::endl;
	}

	return errno;
#endif //__unix__ && BVS_THREAD_NAMES
	(void) threadName;

	return 0;
}
