#ifndef BVS_MODULE_H
#define BVS_MODULE_H

#include "bvs/archutils.h"
#include "bvs/bvsinfo.h"
#include "bvs/config.h"
#include "bvs/connector.h"
#include "bvs/logger.h"
#include "bvs/traits.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** Status possibilities. */
	enum class Status { NONE = 0, OK = 1, WAIT = 2, FAIL = 4, NOINPUT = 8 };



	/** Base class for Modules.
	 * This is the base class for Modules.
	 * Extend this class and implement the functions below.
	 */
	class BVS_PUBLIC Module
	{
		public:
			Module(); /**< Base module constructor. */

			virtual ~Module(); /**< Base module destructor. */
			virtual Status execute() = 0; /**< Execute function doing all the work. */
			virtual Status debugDisplay() = 0; /**< UNUSED */
			virtual Status prepareHotSwap(void*&); /** TODO */

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
 * @param[in] TODO
 */
BVS_PUBLIC void registerModule(const std::string& id, BVS::Module* module, bool hotSwap = false);



#endif //BVS_MODULE_H

