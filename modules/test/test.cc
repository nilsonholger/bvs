#include "test.h"



test::test(const BVSConfig& config)
	: BVSModule(config)
	, identifier("test")
	, logger(identifier)
	, config(config)
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
		BVS::registerModule(identifier, new test(config));
		return 0;
	}
}
