#ifndef BVSEXAMPLEMODULE_H
#define BVSEXAMPLEMODULE_H

#include "bvs/module.h"



/** This is the BVSExampleModule module.
 * Please add sufficient documentation to enable others to use it.
 * Include information about:
 * - Dependencies
 * - Inputs
 * - Outputs
 * - Configuration Options
 */
class BVSExampleModule : public BVS::Module
{
	public:
		/** Your module constructor.
		 * Please do not change the signature, as it will be called by the
		 * framework.
		 * You can use the constructor/destructor pair to create/destroy your data.
		 * @param[in] info Your modules information, will be set by framework.
		 * @param[in] bvs Reference to framework info for e.g. config option retrieval.
		 */
		BVSExampleModule(BVS::ModuleInfo info, const BVS::Info& bvs);

		/** Your module destructor. */
		~BVSExampleModule();

		/** Execute function doing all the work.
		 * This function is executed exactly once and only once upon each started
		 * round/step of the framework. It is supposed to contain the actual work
		 * of your module.
		 */
		BVS::Status execute();

		/** UNUSED
		 * @return Module's status.
		 */
		BVS::Status debugDisplay();

	private:
		const BVS::ModuleInfo info; /**< Your module metadata, set by framework. */
		BVS::Logger logger; /**< Your logger instance. @see Logger */
		const BVS::Info& bvs; /**< Your Info reference. @see Info */

		/** Example Connector used to retrieve/send data from/to other modules.
		 * @see Connector
		 */
		//BVS::Connector<int> input;
		//BVS::Connector<std::string> output;

		BVSExampleModule(const BVSExampleModule&) = delete; /**< -Weffc++ */
		BVSExampleModule& operator=(const BVSExampleModule&) = delete; /**< -Weffc++ */
};



#endif //BVSEXAMPLEMODULE_H

