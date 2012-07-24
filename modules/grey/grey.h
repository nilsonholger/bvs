#ifndef GREY_H
#define GREY_H

#include "bvs/module.h"
#include "opencv2/opencv.hpp"



/** This is the grey class.
 * Please add sufficient documentation to enable others to use it.
 * Include information about:
 * - Dependencies
 * - Inputs
 * - Outputs
 * - Configuration Options
 */
class grey : public BVS::Module
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
		grey(const std::string id, const BVS::Config& config);

		/** Your module destructor. */
		~grey();

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
		const std::string id; /**< Your unique module id, set by framework. */

		/** Your logger instance.
		 * @see BVS::Logger
		 */
		BVS::Logger logger;

		/** Your config instance.
		 * @see BVS::Config
		 */
		const BVS::Config& config;

		/** Example connector used to retrieve/send data from/to other modules.
		 * @see BVS::Connector
		 */
		BVS::Connector<cv::Mat> input;
		//BVS::Connector output;

		cv::Mat frame;

		grey(const grey&) = delete; /**< -Weffc++ */
		grey& operator=(const grey&) = delete; /**< -Weffc++ */
};

#endif //GREY_H

