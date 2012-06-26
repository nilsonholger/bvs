#ifndef BVSMODULEDATA_H
#define BVSMODULEDATA_H

#include<atomic>
#include<map>
#include<memory>
#include<string>
#include<thread>

#include "BVSModule.h"



/** Possible module flags (self explanatory). */
enum class BVSModuleFlag { QUIT = 0, WAIT = 1, RUN = 2 };



/** Module metadata. */
struct BVSModuleData
{
	std::string identifier; /**< Name of module. */
	std::string library; /**< Library to load module from. */
	BVSModule* module; /**< Pointer to the module. */
	void* dlib; /**< Dlib handle to module's lib. */
	std::thread thread; /**< Thread handle of module. */
	bool asThread; /**< Determines if module runs in its own thread. */
	BVSModuleFlag flag; /**< System control flag for module. */
	BVSStatus status; /**< Return Status of module functions. */
};



#endif //BVSMODULEDATA_H

