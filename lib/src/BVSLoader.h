#ifndef BVSLOADER_H
#define BVSLOADER_H

#include<string>
#include<thread>

#include "BVSConfig.h"
#include "BVSControl.h"
#include "BVSLogger.h"
#include "BVSModuleData.h"



// Forward declaration.
class BVSControl;



/** Module Map. */
typedef std::map<std::string, std::shared_ptr<BVSModuleData>, std::less<std::string>> BVSModuleMap;



/** The system loader: loads, unloads and controls modules. */
class BVSLoader
{
	public:
		/** Constructor for loader.
		 * @param[in] control Reference to control mechanism.
		 * @param[in] config Reference to config system.
		 */
		BVSLoader(BVSControl& control, BVSConfig& config);

		/** Registers a module.
		 * @param[in] identifier Name of module.
		 * @param[in] module Pointer to module.
		 */
		static void registerModule(const std::string& identifier, BVSModule* module);

		/** Load the given module.
		 * Executes bvsRegisterModule function in module to register it with the
		 * system.
		 * @param[in] moduleTraits The name and traits of the module.
		 * @param[in] asThread Whether to load the module inside a thread or not.
		 * @return Reference to object.
		 */
		BVSLoader& load(const std::string& moduleTraits, const bool asThread);

		/** Unload the given module.
		 * @param[in] moduleName The name of the module.
		 * @param[in] eraseFromMap Whether to delete all associated metadata.
		 * @return Reference to object.
		 */
		BVSLoader& unload(const std::string& moduleName, const bool eraseFromMap = true);

		/** Unload all modules.
		 * @return Reference to object.
		 */
		BVSLoader& unloadAll();

		/** Connect modules.
		 * Connects all modules by checking desired configuration options and
		 * processing them one by one. This functions actually connects 2
		 * connectors created by modules.
		 * @return Reference to object.
		 */
		BVSLoader& connectModules();

	private:
		/** Map of registered modules and their metadata. */
		static BVSModuleMap modules;

		/** Reference to control mechanism, needed to load und unload threaded modules. */
		BVSControl& control;

		BVSLogger logger; /**< Logger metadata. */
		BVSConfig& config; /**< Config reference. */

		BVSLoader(const BVSLoader&) = delete; /**< -Weffc++ */
		BVSLoader& operator=(const BVSLoader&) = delete; /**< -Weffc++ */

		friend class BVSControl;
};



#endif //BVSLOADER_H

