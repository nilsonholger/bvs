#ifndef BVSEXAMPLEMODULE_H
#define BVSEXAMPLEMODULE_H

#include "BVSModule.h"



// TODO comments, usage examples...
class BVSExampleModule : public BVSModule
{
	public:
		BVSExampleModule(const std::string id, const BVSConfig& config);
		~BVSExampleModule();
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

		BVSConnector input;
		BVSConnector output;

		BVSExampleModule(const BVSExampleModule&) = delete; /**< -Weffc++ */
		BVSExampleModule& operator=(const BVSExampleModule&) = delete; /**< -Weffc++ */
};

#endif //BVSEXAMPLEMODULE_H

