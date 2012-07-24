#ifndef CANNY_H
#define CANNY_H

#include<chrono>
#include "bvs/module.h"
#include "opencv2/opencv.hpp"



/** This is the canny class.
 * Please add sufficient documentation to enable others to use it.
 * Include information about:
 * - Dependencies
 * - Inputs
 * - Outputs
 * - Configuration Options
 */
class canny : public BVS::Module
{
	public:
		/** Your module constructor.
		 * Please do not change the signature, as it will be called by the
		 * framework.
		 * You can use the constructor/destructor pair to create/destroy your data.
		 * @param[in] id Your modules unique identifier, will be set by framework.
		 * @param[in] config Reference to config to enable option retrieval.
		 */
		canny(const std::string id, const BVS::Config& config);

		/** Your module destructor. */
		~canny();

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
		 * @see Logger
		 */
		BVS::Logger logger;

		/** Your config instance.
		 * @see Config
		 */
		const BVS::Config& config;

		/** Example connector used to retrieve/send data from/to other modules.
		 * @see Connector
		 */
		BVS::Connector<cv::Mat> input;
		BVS::Connector<cv::Mat> output;

		cv::Mat frame;
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
		int counter;

		canny(const canny&) = delete; /**< -Weffc++ */
		canny& operator=(const canny&) = delete; /**< -Weffc++ */
};

#endif //CANNY_H

