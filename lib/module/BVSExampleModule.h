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
	//TODO
	struct HotSwapData {
		BVS::Connector<int> i;
		BVS::Connector<string> s;
	};

	public:
		/** Your module constructor.
		 * Please do not change the signature, as it will be called by the
		 * framework.
		 * You can use the constructor/destructor pair to create/destroy your data.
		 * @param[in] id Your modules unique identifier, will be set by framework.
		 * @param[in] bvs Reference to framework info for e.g. config option retrieval.
		 * @param[in] TODO
		 */
		BVSExampleModule(const std::string id, const BVS::Info& bvs);

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

		//TODO
		BVS::Status prepareHotSwap(void*& data);

	private:
		const std::string id; /**< Your unique module id, set by framework. */

		/** Your logger instance.
		 * @see Logger
		 */
		BVS::Logger logger;

		/** Your config system.
		 * @see Config
		 */
		BVS::Config config;

		/** Your Info reference;
		 * @see Info
		 */
		const BVS::Info& bvs;

		/** Example Connector used to retrieve/send data from/to other modules.
		 * @see Connector
		 */
		//BVS::Connector<int> input;
		//BVS::Connector<std::string> output;

		HotSwapData data;

		BVSExampleModule(const BVSExampleModule&) = delete; /**< -Weffc++ */
		BVSExampleModule& operator=(const BVSExampleModule&) = delete; /**< -Weffc++ */
};

#endif //BVSEXAMPLEMODULE_H

