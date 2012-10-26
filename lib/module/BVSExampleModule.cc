#include "BVSExampleModule.h"



// This is your module's constructor.
// Please do not change its signature as it is called by the framework (so the
// framework actually creates your module) and the framework assigns the unique
// identifier and gives you access to its config.
// However, you should use it to create your data structures etc.
BVSExampleModule::BVSExampleModule(const std::string id, const BVS::Info& bvs)
	: BVS::Module(),
	id(id),
	logger(id),
	config("BVSExampleModule", 0, nullptr),
	bvs(bvs)
	//yourSwitch(bvs.config.getValue<bool>(id + ".yourSwitch, false)),
	//input("testIn", BVS::ConnectorType::INPUT),
	//output("testOut", BVS::ConnectorType::OUTPUT)
{

}



// This is your module's destructor.
// See the constructor for more info.
BVSExampleModule::~BVSExampleModule()
{

}



// Put all your work here.
BVS::Status BVSExampleModule::execute()
{
	// to log messages to console or file, use the LOG(...) macro
	//LOG(3, "Execution of " << id << "!");

	// Various settings and information
	// in some config:
	// [thisModuleId]
	// foo = 42
	//int foo = bvs.getValue<int>(id + ".myInteger, 23);
	//unsigned long long round = bvs.round;
	//int lastRoundModuleDuration = bvs.moduleDurations.find(id)->second.count();
	//int lastRoundDuration = bvs.lastRoundDuration.count();

	// Simple Connector Example
	//int incoming;
	//std::string message;
	//if (input.receive(incoming))
	//{
	//	message = "received" + std::to_string(incoming);
	//}
	//else
	//{
	//	message = "no input received!";
	//}
	//output.send(message);

	// Advanced Connector Example (do not forget to unlock the connection or
	// you will cause deadlocks)
	//std::string s2 = "This";
	//output.lockConnection();
	//*output = s2;
	//*output = *output + " is an";
	//s2 = " example!";
	//*output += s2;
	//output.unlockConnection();

	return BVS::Status::OK;
}



// UNUSED
BVS::Status BVSExampleModule::debugDisplay()
{
	return BVS::Status::OK;
}



// This function is called by the framework upon creating a module instance of
// this class. It creates the module and registers it within the framework.
// DO NOT CHANGE OR DELETE
extern "C" {
	int bvsRegisterModule(std::string id, BVS::Info& bvs)
	{
		registerModule(id, new BVSExampleModule(id, bvs));

		return 0;
	}

	int bvsHotSwapModule(std::string id, BVS::Module* module)
	{
		registerModule(id, reinterpret_cast<BVSExampleModule*>(module), true);

		return 0;
	}
}

