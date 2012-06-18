#ifndef BVSEXAMPLEMODULE_H
#define BVSEXAMPLEMODULE_H

#include "BVSModule.h"

class BVSExampleModule : public BVSModule
{
	public:
		BVSExampleModule(const std::string identifier, const BVSConfig& config);
		~BVSExampleModule();
		BVSStatus onLoad();
		BVSStatus preExecute();
		BVSStatus execute();
		BVSStatus postExecute();
		BVSStatus debugDisplay();
		BVSStatus onClose();

	private:
		const std::string identifier;
		BVSLogger logger;
		const BVSConfig& config;
		BVSExampleModule(const BVSExampleModule&) = delete; /**< -Weffc++ */
		BVSExampleModule& operator=(const BVSExampleModule&) = delete; /**< -Weffc++ */
};

#endif //BVSEXAMPLEMODULE_H

