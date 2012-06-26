#ifndef BVSLOADER_H
#define BVSLOADER_H

#include<string>
#include<thread>

#include "BVSConfig.h"
#include "BVSControl.h"
#include "BVSLogger.h"
#include "BVSModuleData.h"



class BVSControl;
/** Module Map. */
typedef std::map<std::string, std::shared_ptr<BVSModuleData>, std::less<std::string>> BVSModuleMap;



/** The system loader: loads, unloads and controls modules. */
class BVSLoader
{
	public:
		/** Constructor for loader.
		 * @param[in] config Reference to config system.
		 */
		// TODO remove config somehow, set config from somewhere else
		BVSLoader(BVSControl& control, BVSConfig& config);

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

