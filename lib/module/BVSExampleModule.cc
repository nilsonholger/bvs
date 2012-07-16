#include "BVSExampleModule.h"



// This is your module's constructor.
// Please do not change its signature as it is called by the framework (so the
// framework actually creates your module) and the framework assigns the unique
// identifier and gives you access to the its config.
// However, you might use it to create your data structures etc., or you can use
// the onLoad() and onClose() functions, just be consistent in order to avoid
// weird errors.
BVSExampleModule::BVSExampleModule(const std::string id, const BVSConfig& config)
	: BVSModule()
	, id(id)
	, logger(id)
	, config(config)
	, input("testIn", BVSConnectorType::IN)
	, output("testOut", BVSConnectorType::OUT)
{

}



// This is your module's destructor.
// See the constructor for more info.
BVSExampleModule::~BVSExampleModule()
{

}



// Executed at module load.
BVSStatus BVSExampleModule::onLoad()
{
LOG(2, "loaded BVSExampleModule module!");

	return BVSStatus::OK;
}



// UNUSED
BVSStatus BVSExampleModule::preExecute()
{
	return BVSStatus::OK;
}



// Put all your work here.
BVSStatus BVSExampleModule::execute()
{
	LOG(2, "Execution of " << id << "!");

	return BVSStatus::OK;
}



// UNUSED
BVSStatus BVSExampleModule::postExecute()
{
	return BVSStatus::OK;
}



BVSStatus BVSExampleModule::debugDisplay()
{
	return BVSStatus::OK;
}



// Executed at module unload.
BVSStatus BVSExampleModule::onClose()
{
	return BVSStatus::OK;
}



// This function is called by the framework upon creating a module instance of
// this class. It creates the module and registers it within the framework.
// DO NOT CHANGE OR DELETE
extern "C" {
	// register with framework
	int bvsRegisterModule(std::string id, BVSConfig& config)
	{
		registerModule(id, new BVSExampleModule(id, config));

		return 0;
	}
}

