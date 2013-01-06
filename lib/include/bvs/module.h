#ifndef BVS_MODULE_H
#define BVS_MODULE_H

#include "bvs/config.h"
#include "bvs/connector.h"
#include "bvs/info.h"
#include "bvs/logger.h"
#include "bvs/traits.h"
#include "bvs/utils.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** Module status or request possibilities. */
	enum class Status {
		OK = 0, /**< Module running OK, all systems are go. Nothing to do here. */
		NOINPUT = 1, /**< Module is waiting for input through on of it's connectors. @todo: NOT YET ACTED UPON */
		FAIL = 2, /**< Module failing to execute properly. @todo: NOT YET ACTED UPON */
		WAIT = 4, /**< Module waiting for event to happen, e.g.\ a certain input or other signal. @todo: NOT YET ACTED UPON */
		DONE = 8, /**< Module is done processing and can be unloaded. */
		SHUTDOWN = 16 /**< Module is requesting system shutdown, e.g.\ due to no more input to process.
						The system will keep on running (including the requesting module) for as many rounds
						as there are modules loaded and then shutdown. */
	};



	/** Base class for Modules.
	 * This is the base class for Modules.
	 * Extend this class and implement the functions below.
	 */
	class BVS_PUBLIC Module
	{
		public:
			Module() = default; /**< Base module constructor. */

			virtual ~Module() { } /**< Base module destructor. */
			virtual Status execute() = 0; /**< Execute function doing all the work. */
			virtual Status debugDisplay() = 0; /**< UNUSED */

		private:
			Module(const Module&) = delete; /**< -Weffc++ */
			Module& operator=(const Module&) = delete; /**< -Weffc++ */

	};
} // namespace BVS



/** Register a Module with the framework.
 * This registers a module within the framework.
 * It should not be called by the module itself, but will instead be called
 * by the system's module loader upon loading the module.
 * @param[in] id The Module's name.
 * @param[in] module A pointer to the module.
 * @param[in] hotSwap Whether to use module Hot Swapping instead of creating a new Module (please see BVS::Loader).
 */
BVS_PUBLIC void registerModule(const std::string& id, BVS::Module* module, bool hotSwap = false);



/** @def BVS_MODULE_UTILITIES(BVS_MODULE_CLASS_NAME)
 * Module utility macro.
 * This macro should be called by a module with its class name to create the
 * functions needed by the framework to create/load the module and possibly do
 * further things with/to it.
 *
 * \a BVS_MODULE_CLASS_NAME Name of module class.
 */
#define BVS_MODULE_UTILITIES(BVS_MODULE_CLASS_NAME) \
	extern "C" { \
		int bvsRegisterModule(BVS::ModuleInfo info, BVS::Info& bvs) \
		{ \
			registerModule(info.id, new BVS_MODULE_CLASS_NAME(info, bvs)); \
			return 0; \
		} \
		int bvsHotSwapModule(std::string id, BVS::Module* module) \
		{ \
			registerModule(id, reinterpret_cast<BVS_MODULE_CLASS_NAME*>(module), true); \
			return 0; \
		} \
	}



#endif //BVS_MODULE_H

