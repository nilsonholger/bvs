#ifndef BVSMASTER_H
#define BVSMASTER_H

#include<atomic>
#include<condition_variable>
#include<map>
#include<mutex>
#include<string>
#include<thread>

#include "BVSConfig.h"
#include "BVSLogger.h"
#include "BVSModule.h"
#include "BVSTraits.h"



// Forward declaration
struct BVSModuleData;



// TODO
enum class BVSSystemFlag { QUIT = 0, PAUSE = 1, RUN = 2, STEP = 3, STEP_BACK = 4 };
enum class BVSModuleFlag { QUIT = 0, WAIT = 1, RUN = 2 };



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

		/** Load the given module.
		 * Executes bvsRegisterModule function in module to register it with the
		 * system.
		 * @param[in] identifier The name of the module.
		 * @param[in] asThread Whether to load the module inside a thread or not.
		 * @return Reference to object.
		 */
		BVSMaster& load(const std::string& identifier, bool asThread);

		// TODO comment
		BVSMaster& control(const BVSSystemFlag controlFlag = BVSSystemFlag::PAUSE);

		// TODO comment
		BVSMaster& masterController(const bool forkMasterController = true);

		/** Unload the given module.
		 * @param[in] moduleName The name of the module.
		 * @return Reference to object.
		 */
		BVSMaster& unload(const std::string& moduleName, const bool eraseFromMap = true);

		/** Unload all modules.
		 * @return Reference to object.
		 */
		BVSMaster& unloadAll();

	private:
		/** Controls given module.
		 * @param[in] data Module meta data.
		 * @return Reference to object.
		 */
		BVSMaster& moduleController(BVSModuleData& data);

		/** Controls a module started as a thread.
		 * @param[in] data Module meta data.
		 * @return Reference to object.
		 */
		BVSMaster& threadController(std::shared_ptr<BVSModuleData> data);

		// TODO comment
		BVSSystemFlag flag;

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

		std::thread controlThread;

		long long round;

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
	BVSModuleFlag flag; /**< System control flag for module. */
	BVSStatus status; /**< Return Status of module functions. */
};



#endif //BVSMASTER_H

