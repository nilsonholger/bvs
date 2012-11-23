#ifndef BVS_LOADER_H
#define BVS_LOADER_H

#include <functional>
#include <stack>
#include <string>
#include <thread>

#include "bvs/config.h"
#include "bvs/info.h"
#include "bvs/logger.h"
#include "controldata.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** The system loader: loads and unloads and modules and libraries. */
	class Loader
	{
		public:
			/** Constructor for loader.
			 * @param[in] info Reference to info struct.
			 * @param[in] errorHandler Callback in case an error has occured.
			 */
			Loader(const Info& info, std::function<void()> errorHandler = [](){ exit(0); });

			/** Registers a module.
			 * @param[in] id Name of module.
			 * @param[in] module Pointer to module.
			 * @param[in] hotSwap Whether to hotSwap (an already loaded) module.
			 */
			static void registerModule(const std::string& id, Module* module, bool hotSwap = false);

			/** Load the given module.
			 * Executes bvsRegisterModule function in module to register it with the
			 * system.
			 * If a pool name is given, asThread has no effect.
			 * @param[in] id The module id to give to the new loaded module.
			 * @param[in] library The library to load the module from.
			 * @param[in] configuration The configuration to pass to the module.
			 * @param[in] options The module options (connector settings...).
			 * @param[in] asThread Whether to load the module inside a thread or not.
			 * @param[in] poolName Select, if desired, the module pool to run this module.
			 * @return Reference to object.
			 */
			Loader& load(const std::string& id, const std::string& library, const std::string& configuration, const std::string& options, const bool asThread, const std::string& poolName = std::string());

			/** Unload the given module.
			 * @param[in] moduleName The name of the module.
			 * @return Reference to object.
			 */
			Loader& unload(const std::string& moduleName);

			/** Connect all modules.
			 * @param[in] connectorTypeMatching Whether to try to match associated
			 * connectors.
			 * @return Reference to object.
			 */
			Loader& connectAllModules(const bool connectorTypeMatching = true);

			/** Connect selected module.
			 * Connects the selected module by checking its desired configuration
			 * options and processing them piece by piece.
			 * @param[in] id Module id.
			 * @param[in] connectorTypeMatching Whether to try to match associated
			 * connectors.
			 * @return Reference to object.
			 */
			Loader& connectModule(const std::string& id, const bool connectorTypeMatching = true);

			/** Disconnect selected module.
			 * Disconnects the selected module by disconnecting its connectors one
			 * by one.
			 * @param[in] id Module id.
			 * @return Reference to object.
			 */
			Loader& disconnectModule(const std::string& id);

#ifdef BVS_MODULE_HOTSWAP
			/** HotSwap a module.
			 * This will reload/hotswap an already existing module.
			 *
			 * \note Only works if there is only ONE module instance of a
			 * library present.
			 *
			 * \warning There is a known issue with BVS::Connector. If you
			 * create two connectors of different template types, hotswapping
			 * will fail (due to yet unknown and rather obscure reasons).  As a
			 * possible workaround create a 'dummy' connector of the first type
			 * (same template argument, same connector type). There might be
			 * other combinations that might fail, but these too can possibly
			 * be mitigated by the described workaround.
			 * \par
			 * \warning This has its (fair) limitations. As long as one does not
			 * change the general layout of a module (add/remove/reorder
			 * members) it *should* work fine.  Hotswap will be achieved by
			 * using a 'reinterpret_cast<...>' inside the module. This is
			 * inherently insafe and can fail due to all kind of reasons. YOU
			 * HAVE BEEN WARNED!!!
			 * @param[in] id Module ID to hotswap (if it does not exist, it fails silently).
			 */
			Loader& hotSwapModule(const std::string& id);
#endif //BVS_MODULE_HOTSWAP

			/** Load the necessary library.
			 * @param[in] id The module id of which to load the library.
			 * @param[in] library The base name of the library to load from (no 'lib' and extension).
			 * @return Reference to object.
			 */
			LibHandle loadLibrary(const std::string& id, const std::string& library);

			/** Unload the necessary library.
			 * @param[in] id The name of the module.
			 * @return Reference to object.
			 */
			Loader& unloadLibrary(const std::string& id);

			/** Map of registered modules and their metadata. */
			static ModuleDataMap modules;

		private:
			/** Check input Connector.
			 * Checks input connector for existence, type etc.
			 * @param[in] module Module data for selected module.
			 * @param[in] inputName Name of desired input.
			 * @return True if all checks passed.
			 */
			Loader& checkModuleInput(const ModuleData* module, const std::string& inputName);

			/** Check output Connector.
			 * Checks output Connector for existence, type etc.
			 * @param[in] module Module data for selected module.
			 * @param[in] targetModule Name of desired module.
			 * @param[in] targetOutput Name of desried output.
			 */
			Loader& checkModuleOutput(const ModuleData* module, const std::string& targetModule, const std::string& targetOutput);

			/** Print Connector list.
			 * Prints a list of all Connectors defined by given module.
			 * @param[in] module Module data for selected module.
			 * @return Reference to object.
			 */
			Loader& printModuleConnectors(const ModuleData* module);

			Logger logger; /**< Logger metadata. */
			const Info& info; /**< Info reference. */
			std::function<void()> errorHandler; /**< Function to call on errors. */
			static ModuleVector* hotSwapGraveYard; /** GraveYard for hotswapped module pointers. */

			Loader(const Loader&) = delete; /**< -Weffc++ */
			Loader& operator=(const Loader&) = delete; /**< -Weffc++ */
	};
} // namespace BVS



#endif //BVS_LOADER_H

