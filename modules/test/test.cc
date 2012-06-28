#include "test.h"



test::test(const std::string identifier, const BVSConfig& config)
	: BVSModule(config)
	, identifier(identifier)
	, logger(identifier)
	, config(config)
	, input("testIn", BVSConnectorType::IN)
	, output("testOut", BVSConnectorType::OUT)
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

	int* in = new int(42);
	output.set(in);
	int* foobar;
	foobar = input.get<int>();
	LOG(0, "getting: " << *foobar);

	//*output.data = 5;
	//LOG(0, "getting something: " << *input.data);
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
		registerModule(identifier, new test(identifier, config));

		return 0;
	}
}

