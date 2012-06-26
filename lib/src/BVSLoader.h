#ifndef BVSLOADER_H
#define BVSLOADER_H

#include<string>
#include<thread>

#include "BVSConfig.h"
#include "BVSLogger.h"
#include "BVSModuleData.h"


// TODO maybe the loader needs to know about the control to start the threads and signal quit to modules

/** The system loader: loads, unloads and controls modules. */
class BVSLoader
{
	public:
		/** Constructor for loader.
		 * @param[in] config Reference to config system.
		 */
		// TODO remove somehow, set from somewhere else
		BVSLoader(BVSConfig& config);

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
		BVSLoader& load(const std::string& identifier, bool asThread);

		/** Unload the given module.
		 * @param[in] moduleName The name of the module.
		 * @return Reference to object.
		 */
		BVSLoader& unload(const std::string& moduleName, const bool eraseFromMap = true);

		/** Unload all modules.
		 * @return Reference to object.
		 */
		BVSLoader& unloadAll();

	private:
		BVSLogger logger; /**< Logger metadata. */
		BVSConfig& config; /**< Config reference. */

		BVSLoader(const BVSLoader&) = delete; /**< -Weffc++ */
		BVSLoader& operator=(const BVSLoader&) = delete; /**< -Weffc++ */
};



#endif //BVSLOADER_H

