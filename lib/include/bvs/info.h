#ifndef BVS_INFO_H
#define BVS_INFO_H

#include <chrono>
#include <map>
#include <string>

#include "bvs/config.h"
#include "bvs/traits.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** Info meta data stuff. */
	struct BVS_PUBLIC Info
	{
		/** Reference to config system. */
		const Config& config;

		/** Round(evolution/step/generation) number. */
		unsigned long long round;

		/** Duration of last round. */
		std::chrono::duration<unsigned int, std::milli> lastRoundDuration;

		/** Module durations of last round. */
		std::map<std::string, std::chrono::duration<unsigned int, std::milli>> moduleDurations;

		/** Calculate frames per second.
		 * @return FPS as string.
		 */
		std::string getFPS() const;
	};



	/** Module information. */
	struct BVS_PUBLIC ModuleInfo
	{
		std::string id; /**< Module id. */
		std::string conf; /**< Module Configuration. */
	};
}



#endif // BVS_INFO_H

