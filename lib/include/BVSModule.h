#ifndef BVSMODULE_H
#define BVSMODULE_H

#include "BVSConfig.h"
#include "BVSConnector.h"
#include "BVSLogger.h"



/** BVSStatus possibilities. */
enum class BVSStatus { NONE = 0, OK = 1, WAIT = 2, FAIL = 4, NOINPUT = 8 };



/** Base class for BVS Modules.
 * This is the base class for BVS Modules.
 * Extend this class and implement the functions below.
 */
class BVSModule
{
	// TODO comments
	public:
		BVSModule(const BVSConfig& config); /**< Constructor of Module Base. */
		virtual ~BVSModule(); /**< Destructor of Module Base. */
		virtual BVSStatus onLoad() = 0;
		virtual BVSStatus preExecute() = 0;
		virtual BVSStatus execute() = 0;
		virtual BVSStatus postExecute() = 0;
		virtual BVSStatus debugDisplay() = 0;
		virtual BVSStatus onClose() = 0;

	protected:
		const BVSConfig& config;

	private:
		BVSModule(const BVSModule&) = delete; /**< -Weffc++ */
		BVSModule& operator=(const BVSModule&) = delete; /**< -Weffc++ */

};



/** Register a Module with the BVS framework.
 * This registers a module within the BVS framework.
 * It should not be called by the module itself, but will instead be called
 * by the system's module loader upon loading the module.
 * @param[in] identifier The Module's name.
 * @param[in] module A pointer to the module.
 */
void registerModule(const std::string& identifier, BVSModule* module);



#endif //BVSMODULE_H

