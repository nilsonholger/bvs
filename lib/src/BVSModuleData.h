#ifndef BVSMODULEDATA_H
#define BVSMODULEDATA_H

#include<atomic>
#include<map>
#include<memory>
#include<string>
#include<thread>

#include "BVSModule.h"



// TODO
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



/** Module Map. */
typedef std::map<std::string, std::shared_ptr<BVSModuleData>, std::less<std::string>> BVSModuleMap;



// TODO move into class or whatever or somewhere else to avoid useless warnings from some translation units
/** Map of registered modules and their metadata. */
static BVSModuleMap modules;

// TODO comment
static std::atomic<int> runningThreads;
static int threadedModules = 0;

#endif //BVSMODULEDATA_H

