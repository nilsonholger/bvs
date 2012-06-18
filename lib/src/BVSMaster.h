#ifndef BVSMASTER_H
#define BVSMASTER_H

#include<atomic>
#include<cstdlib>
#include<condition_variable>
#include<iostream>
#include<list>
#include<map>
#include<mutex>
#include<string>
#include<thread>
#include<vector>
#include<dlfcn.h>

#include "BVSConfig.h"
#include "BVSLogger.h"
#include "BVSModule.h"
#include "BVSTraits.h"



// Forward declaration
struct BVSModuleData;



// TODO
enum class BVSFlag { QUIT = 0, NOOP = 1, RUN = 2, STEP = 3, STEP_BACK = 4 };



/** Module List. */
typedef std::map<std::string, std::shared_ptr<BVSModuleData>, std::less<std::string>> BVSModuleMap;



/** The system master: loads, unloads and controls modules. */
class BVSMaster
{
	public:
		/** Constructor for master.
		 * @param[in] config Reference to config system.
		 */
		BVSMaster(BVSConfig& config);

		/** Registers a module.
		 * @param[in] data Module meta data.
		 */
		static void registerModule(const std::string& identifier, BVSModule* module);

		/** Load the given module, executes bvsRegisterModule function in module
		 * to register it with the system.
		 * @param[in] identifier The name of the module.
		 * @param[in] asThread Whether to load the module inside a thread or not.
		 * @return Reference to object.
		 */
		BVSMaster& load(const std::string& identifier, bool asThread);

		// TODO main control thread
		BVSMaster& control();

		/** Controls given module.
		 * TODO explain important difference inside function if called by threaded
		 * module
		 * @param[in] data Module meta data.
		 * @return Reference to object.
		 */
		BVSMaster& moduleController(std::shared_ptr<BVSModuleData> data);

		/** Unload the given module.
		 * @param[in] moduleName The name of the module.
		 * @return Reference to object.
		 */
		BVSMaster& unload(const std::string& moduleName);

		/** Unload all modules.
		 * @return Reference to object.
		 */
		BVSMaster& unloadAll();

		BVSFlag flag;

	private:
		/** Map of registered modules and their metadata. */
		static BVSModuleMap modules;

		BVSLogger logger; /**< Logger metadata. */
		BVSConfig& config; /**< Config reference. */

		// TODO comment
		std::atomic<int> runningThreads;
		int threadedModules;

		std::mutex masterMutex;
		std::unique_lock<std::mutex> masterLock;
		std::condition_variable masterCond;

		std::mutex threadMutex;
		std::condition_variable threadCond;

		BVSMaster(const BVSMaster&) = delete; /**< -Weffc++ */
		BVSMaster& operator=(const BVSMaster&) = delete; /**< -Weffc++ */
};



/** Module metadata. */
struct BVSModuleData
{
	std::string identifier; /**< Name of module. */
	std::string library; /**< Library to load module from. */
	BVSModule* module; /**< Pointer to the module. */
	void* dlib; /**< Dlib handle to module's lib. */
	std::thread thread; /**< Thread handle of module. */
	bool asThread; /**< Determines if module runs in its own thread. */
	BVSFlag flag; /**< System control flag for module. */
	BVSStatus status; /**< Return Status of module functions. */
};



#endif //BVSMASTER_H

