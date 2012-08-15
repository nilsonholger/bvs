#ifndef BVS_INFO_H
#define BVS_INFO_H


#include<chrono>

#include "bvs/config.h"



//TODO comments
namespace BVS
{
	struct Info
	{
		const Config& config;
		unsigned int round;
		unsigned int lastRoundDuration;
	};
}

#endif // BVS_INFO_H

