#ifndef TEST2_H
#define TEST2_H

#include "BVSModule.h"



class test2 : public BVSModule
{
	public:
		test2(const std::string id, const BVSConfig& config);
		~test2();
		BVSStatus onLoad();
		BVSStatus preExecute();
		BVSStatus execute();
		BVSStatus postExecute();
		BVSStatus debugDisplay();
		BVSStatus onClose();

	private:
		const std::string id;
		BVSLogger logger;
		const BVSConfig& config;

		BVSConnector<std::string> input;
		BVSConnector<int> output;

		test2(const test2&) = delete; /**< -Weffc++ */
		test2& operator=(const test2&) = delete; /**< -Weffc++ */
};

#endif //TEST2_H

