#include "BVSExampleModule.h"



BVSExampleModule::BVSExampleModule(const std::string id, const BVSConfig& config)
	: BVSModule(config)
	, id(id)
	, logger(id)
	, config(config)
	, input("testIn", BVSConnectorType::IN)
	, output("testOut", BVSConnectorType::OUT)
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
	LOG(2, "Execution of " << id << "!");

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
	int bvsRegisterModule(std::string id, BVSConfig& config)
	{
		registerModule(id, new BVSExampleModule(id, config));

		return 0;
	}
}

