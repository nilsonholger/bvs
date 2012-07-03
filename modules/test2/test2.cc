#include "test2.h"



test2::test2(const std::string id, const BVSConfig& config)
	: BVSModule(config)
	, id(id)
	, logger(id)
	, config(config)
	, input("testIn", BVSConnectorType::IN)
	, output("testOut", BVSConnectorType::OUT)
{

}



test2::~test2()
{

}



BVSStatus test2::onLoad()
{
LOG(2, "loaded test2 module!");

	return BVSStatus::OK;
}



BVSStatus test2::preExecute()
{
	return BVSStatus::OK;
}



BVSStatus test2::execute()
{
	LOG(2, "Execution of " << id << "!");

	int foo = rand();
	//std::string foo = "some string data!";
	//int* in = new int(42);
	//output.set(in);
	output.set() = foo;
	foo++;
	std::string foobar;
	foobar = input.get();
	LOG(0, "getting: " << foobar);

	return BVSStatus::OK;
}



BVSStatus test2::postExecute()
{
	return BVSStatus::OK;
}



BVSStatus test2::debugDisplay()
{
	return BVSStatus::OK;
}



BVSStatus test2::onClose()
{
	return BVSStatus::OK;
}



extern "C" {
	// register with framework
	int bvsRegisterModule(std::string id, BVSConfig& config)
	{
		registerModule(id, new test2(id, config));

		return 0;
	}
}

