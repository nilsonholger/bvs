#include "BVSExampleModule.h"



// This is your module's constructor.
// Please do not change its signature as it is called by the framework (so the
// framework actually creates your module) and the framework assigns the unique
// identifier and gives you access to the its config.
// However, you might use it to create your data structures etc., or you can use
// the onLoad() and onClose() functions, just be consistent in order to avoid
// weird errors.
BVSExampleModule::BVSExampleModule(const std::string id, const BVS::Config& config)
	: BVS::Module()
	, id(id)
	, logger(id)
	, config(config)
	, input("testIn", BVS::ConnectorType::INPUT)
	, output("testOut", BVS::ConnectorType::OUTPUT)
{

}



// This is your module's destructor.
// See the constructor for more info.
BVSExampleModule::~BVSExampleModule()
{

}



// Executed at module load.
BVS::Status BVSExampleModule::onLoad()
{
	LOG(2, "loaded BVSExampleModule module!");

	return BVS::Status::OK;
}



// UNUSED
BVS::Status BVSExampleModule::preExecute()
{
	return BVS::Status::OK;
}



// Put all your work here.
BVS::Status BVSExampleModule::execute()
{
	LOG(2, "Execution of " << id << "!");

	return BVS::Status::OK;
}



// UNUSED
BVS::Status BVSExampleModule::postExecute()
{
	return BVS::Status::OK;
}



BVS::Status BVSExampleModule::debugDisplay()
{
	return BVS::Status::OK;
}



// Executed at module unload.
BVS::Status BVSExampleModule::onClose()
{
	return BVS::Status::OK;
}



// This function is called by the framework upon creating a module instance of
// this class. It creates the module and registers it within the framework.
// DO NOT CHANGE OR DELETE
extern "C" {
	// register with framework
	int bvsRegisterModule(std::string id, BVS::Config& config)
	{
		registerModule(id, new BVSExampleModule(id, config));

		return 0;
	}
}

