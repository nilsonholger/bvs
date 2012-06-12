#include "BVSExampleModule.h"



BVSExampleModule::BVSExampleModule(const BVSConfig& config)
	: BVSModule(config)
	, identifier("BVSExampleModule")
	, logger(identifier)
	, config(config)
{

}



BVSExampleModule::~BVSExampleModule()
{

}



BVSStatus BVSExampleModule::onLoad()
{
LOG(2, "loaded BVSExampleModule module!");

	return BVSStatus::OK;
}



BVSStatus BVSExampleModule::preExecute()
{
	return BVSStatus::OK;
}



BVSStatus BVSExampleModule::execute()
{
	LOG(2, "Execution of " << identifier << "!");

	return BVSStatus::OK;
}



BVSStatus BVSExampleModule::postExecute()
{
	return BVSStatus::OK;
}



BVSStatus BVSExampleModule::debugDisplay()
{
	return BVSStatus::OK;
}



BVSStatus BVSExampleModule::onClose()
{
	return BVSStatus::OK;
}



extern "C" {
	// register with framework
	int bvsRegisterModule(std::string identifier, BVSConfig& config)
	{
		BVS::registerModule(identifier, new BVSExampleModule(config));
		return 0;
	}
}
