#include "input.h"



// This is your module's constructor.
// Please do not change its signature as it is called by the framework (so the
// framework actually creates your module) and the framework assigns the unique
// identifier and gives you access to the its config.
// However, you might use it to create your data structures etc., or you can use
// the onLoad() and onClose() functions, just be consistent in order to avoid
// weird errors.
input::input(const std::string id, const BVS::Config& config)
	: BVS::Module()
	, id(id)
	, logger(id)
	, config(config)
	, output("image", BVS::ConnectorType::OUTPUT)
	, capture()
{
	capture.open(0);
	if(!capture.isOpened()) exit(1);

	capture.set(CV_CAP_PROP_MODE, 0);
	//capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	//capture.set(CV_CAP_PROP_FRAME_HEIGHT, 800);
	capture.set(CV_CAP_PROP_FPS, 30);
}



// This is your module's destructor.
// See the constructor for more info.
input::~input()
{

}



// Put all your work here.
BVS::Status input::execute()
{
	LOG(2, "Execution of " << id << "!");

	capture >> *output;

	return BVS::Status::OK;
}



BVS::Status input::debugDisplay()
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
		registerModule(id, new input(id, config));

		return 0;
	}
}

