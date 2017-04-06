#include "BVSExampleModule.h"



// This is your module's constructor.
// Please do not change its signature as it is called by the framework (so the
// framework actually creates your module) and the framework assigns the unique
// identifier and gives you access to configuration data.
BVSExampleModule::BVSExampleModule(BVS::ModuleInfo info, const BVS::Info& bvs)
	: BVS::Module()
	, info(info)
	, logger(info.id)
	, bvs(bvs)
	//, input("inputName", BVS::ConnectorType::INPUT)
	//, output("outputName", BVS::ConnectorType::OUTPUT)

	// CONFIGURATION RETRIEVAL
	//, yourSwitch(bvs.config.getValue<bool>(info.conf + ".yourSwitch", false))
{

}



// This is your module's destructor.
BVSExampleModule::~BVSExampleModule() noexcept
{

}



// Put all your work here.
BVS::Status BVSExampleModule::execute()
{
	// LOGGING: use the LOG(...) macro
	//LOG(3, "Execution of " << info.id << "!");

	// VARIOUS INFORMATION FROM BVS
	//unsigned long long round = bvs.round;
	//int lastRoundModuleDuration = bvs.moduleDurations.find(info.id)->second.count();
	//int lastRoundDuration = bvs.lastRoundDuration.count();

	// CONNECTOR USAGE: it is always a good idea to check input, twice
	//int incoming;
	//if (!input.receive(incoming)) return BVS::Status::NOINPUT;
	//if (incoming==int()) return BVS::Status::NOINPUT;
	//std::string message = "received " + std::to_string(incoming);
	//output.send(message);

	return BVS::Status::OK;
}



// UNUSED
BVS::Status BVSExampleModule::debugDisplay()
{
	return BVS::Status::OK;
}



/** This calls a macro to create needed module utilities. */
BVS_MODULE_UTILITIES(BVSExampleModule)

