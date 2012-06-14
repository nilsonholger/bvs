#ifndef BVSMASTER_H
#define BVSMASTER_H

#include<cstdlib>
#include<condition_variable>
#include<iostream>
#include<list>
#include<map>
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

	private:
		/** Map of registered modules and their metadata. */
		static BVSModuleMap modules;

		//std::condition_variable controller;
		//std::mutex threadMutex;

		BVSLogger logger; /**< Logger metadata. */
		BVSConfig& config; /**< Config reference. */

		BVSMaster(const BVSMaster&) = delete; /**< -Weffc++ */
		BVSMaster& operator=(const BVSMaster&) = delete; /**< -Weffc++ */
};



/** Module metadata. */
struct BVSModuleData
{
	std::string name; /**< Name of module. */
	BVSModule* module; /**< Pointer to the module. */
	std::string library; /**< Library to load module from. */
	void* dlib; /**< Dlib handle to module's lib. */
	bool asThread; /**< Determines if module runs in its own thread. */
	std::thread thread; /**< Thread handle of module. */
};



#endif //BVSMASTER_H

