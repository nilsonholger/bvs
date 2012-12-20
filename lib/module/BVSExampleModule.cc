#include "BVSExampleModule.h"



// This is your module's constructor.
// Please do not change its signature as it is called by the framework (so the
// framework actually creates your module) and the framework assigns the unique
// identifier and gives you access to its config.
// However, you should use it to create your data structures etc.
BVSExampleModule::BVSExampleModule(BVS::ModuleInfo info, const BVS::Info& bvs)
	: BVS::Module(),
	info(info),
	logger(info.id),
	bvs(bvs)
	//yourSwitch(bvs.config.getValue<bool>(info.conf + ".yourSwitch, false)),
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
	//LOG(3, "Execution of " << info.id << "!");

	// VARIOUS INFORMATION FROM BVS
	/*
	unsigned long long round = bvs.round;
	int lastRoundModuleDuration = bvs.moduleDurations.find(info.id)->second.count();
	int lastRoundDuration = bvs.lastRoundDuration.count();
	*/

	// SIMPLE CONNECTOR EXAMPLE (it is always a good idea to check input, twice)
	/*
	int incoming;
	if (!input.receive(incoming)) return BVS::Status::NOINPUT;
	if (incoming==int()) return BVS::Status::NOINPUT;

	std::string message = "received " + std::to_string(incoming);
	output.send(message);
	*/

	// ADVANCED CONNECTOR EXAMPLE (not unlocking the connection will cause deadlock!)
	/*
	output.lockConnection();
	*output = "This";
	*output = *output + " is an";
	*output += "example!";
	output.unlockConnection();
	*/

	return BVS::Status::OK;
}



// UNUSED
BVS::Status BVSExampleModule::debugDisplay()
{
	return BVS::Status::OK;
}



/** This calls a macro to create needed module utilities. */
BVS_MODULE_UTILITIES(BVSExampleModule)

