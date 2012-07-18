#ifndef BVSEXAMPLEMODULE_H
#define BVSEXAMPLEMODULE_H

#include "BVSModule.h"



/** This is the BVSExampleModule class.
 * Please add sufficient documentation to enable others to use it.
 * Include information about:
 * - Dependencies
 * - Inputs
 * - Outputs
 * - Configuration Options
 */
class BVSExampleModule : public BVSModule
{
	public:
		/** Your module constructor.
		 * Please do not change the signature, as it will be called by the
		 * framework.
		 * You can use the constructor/destructor pair or onLoad/onClose functions
		 * to create/destroy your data etc. depending on your specific needs, e.g.
		 * immediate vs. delayed initialization.
		 * @param[in] id Your modules unique identifier, will be set by framework.
		 * @param[in] config Reference to config to enable option retrieval.
		 */
		BVSExampleModule(const std::string id, const BVSConfig& config);

		/** Your module destructor. */
		~BVSExampleModule();

		/** Executed at module load.
		 * This function is called upon loading the module and can/should be
		 * used to initialize your data structures etc.
		 * @return Module's status.
		 */
		BVSStatus onLoad();

		/** UNUSED
		 * @return Module's status.
		 */
		BVSStatus preExecute();

		/** Execute function doing all the work.
		 * This function is executed exactly once and only once upon each started
		 * round/step of the framework. It is supposed to contain the actual work
		 * of your module.
		 */
		BVSStatus execute();

		/** UNUSED
		 * @return Module's status.
		 */
		BVSStatus postExecute();

		/** UNUSED
		 * @return Module's status.
		 */
		BVSStatus debugDisplay();

		/** Executed at module unload.
		 * This function complements the onLoad() function. It is called upon
		 * closing the module the module and can/should be used to
		 * deinitialize your data structures etc.
		 * @return Module's status.
		 */
		BVSStatus onClose();

	private:
		const std::string id; /**< Your unique module id, set by framework. */

		/** Your logger instance.
		 * @see BVSLogger
		 */
		BVSLogger logger;

		/** Your config instance.
		 * @see BVSConfig
		 */
		const BVSConfig& config;

		/** Example connector used to retrieve/send data from/to other modules.
		 * @see BVSConnector
		 */
		BVSConnector<int> input;
		BVSConnector<std::string> output;

		BVSExampleModule(const BVSExampleModule&) = delete; /**< -Weffc++ */
		BVSExampleModule& operator=(const BVSExampleModule&) = delete; /**< -Weffc++ */
};

#endif //BVSEXAMPLEMODULE_H

