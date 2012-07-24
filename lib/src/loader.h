#ifndef BVSLOADER_H
#define BVSLOADER_H

#include<string>
#include<thread>

#include "bvs/config.h"
#include "bvs/logger.h"
#include "control.h"
#include "moduledata.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	// Forward declaration.
	class Control;



	/** Module Map. */
	typedef std::map<std::string, std::shared_ptr<ModuleData>, std::less<std::string>> ModuleMap;



	/** The system loader: loads, unloads and controls modules. */
	class Loader
	{
		public:
			/** Constructor for loader.
			 * @param[in] control Reference to control mechanism.
			 * @param[in] config Reference to config system.
			 */
			Loader(Control& control, Config& config);

			/** Registers a module.
			 * @param[in] id Name of module.
			 * @param[in] module Pointer to module.
			 */
			static void registerModule(const std::string& id, Module* module);

			/** Load the given module.
			 * Executes bvsRegisterModule function in module to register it with the
			 * system.
			 * @param[in] moduleTraits The name and traits of the module.
			 * @param[in] asThread Whether to load the module inside a thread or not.
			 * @return Reference to object.
			 */
			Loader& load(const std::string& moduleTraits, const bool asThread);

			/** Unload the given module.
			 * @param[in] moduleName The name of the module.
			 * @param[in] eraseFromMap Whether to delete all associated metadata.
			 * @return Reference to object.
			 */
			Loader& unload(const std::string& moduleName, const bool eraseFromMap = true);

			/** Unload all modules.
			 * @return Reference to object.
			 */
			Loader& unloadAll();

			/** Connect modules.
			 * Connects all modules by checking desired configuration options and
			 * processing them one by one. This functions actually connects 2
			 * connectors created by modules.
			 * @return Reference to object.
			 */
			Loader& connectModules();

		private:
			/** Map of registered modules and their metadata. */
			static ModuleMap modules;

			/** Reference to control mechanism, needed to load und unload threaded modules. */
			Control& control;

			Logger logger; /**< Logger metadata. */
			Config& config; /**< Config reference. */

			Loader(const Loader&) = delete; /**< -Weffc++ */
			Loader& operator=(const Loader&) = delete; /**< -Weffc++ */

			friend class Control;
	};
} // namespace BVS



#endif //BVSLOADER_H

