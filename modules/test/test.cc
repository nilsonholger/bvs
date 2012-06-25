#include "test.h"



test::test(const std::string identifier, const BVSConfig& config)
	: BVSModule(config)
	, identifier(identifier)
	, logger(identifier)
	, config(config)
	, input(identifier, "testIn", BVSConnectorType::IN)
	, output(identifier, "testOut", BVSConnectorType::OUT)
{

}



test::~test()
{

}



BVSStatus test::onLoad()
{
LOG(2, "loaded test module!");

	return BVSStatus::OK;
}



BVSStatus test::preExecute()
{
	return BVSStatus::OK;
}



BVSStatus test::execute()
{
	LOG(2, "Execution of " << identifier << "!");

	return BVSStatus::OK;
}



BVSStatus test::postExecute()
{
	return BVSStatus::OK;
}



BVSStatus test::debugDisplay()
{
	return BVSStatus::OK;
}



BVSStatus test::onClose()
{
	return BVSStatus::OK;
}



extern "C" {
	// register with framework
	int bvsRegisterModule(std::string identifier, BVSConfig& config)
	{
		BVS::registerModule(identifier, new test(identifier, config));

		return 0;
	}
}
