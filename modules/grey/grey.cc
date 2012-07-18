#include "grey.h"



// This is your module's constructor.
// Please do not change its signature as it is called by the framework (so the
// framework actually creates your module) and the framework assigns the unique
// identifier and gives you access to the its config.
// However, you might use it to create your data structures etc., or you can use
// the onLoad() and onClose() functions, just be consistent in order to avoid
// weird errors.
grey::grey(const std::string id, const BVSConfig& config)
	: BVSModule()
	, id(id)
	, logger(id)
	, config(config)
	, input("input", BVSConnectorType::INPUT)
	//, output("testOut", BVSConnectorType::OUT)
	, frame()
{

}



// This is your module's destructor.
// See the constructor for more info.
grey::~grey()
{

}



// Executed at module load.
BVSStatus grey::onLoad()
{
LOG(2, "loaded grey module!");

	return BVSStatus::OK;
}



// UNUSED
BVSStatus grey::preExecute()
{
	return BVSStatus::OK;
}



// Put all your work here.
BVSStatus grey::execute()
{
	LOG(2, "Execution of " << id << "!");

	frame = input.get();
	LOG(0, frame.total());
	if (frame.total() == 0) return BVSStatus::OK;
	//cv::cvtColor(frame, frame, CV_BGR2GRAY);
	cv::namedWindow("grey", 1);
	cv::imshow("grey", frame);
	//cv::imwrite("foo.bmp", frame);

	return BVSStatus::OK;
}



// UNUSED
BVSStatus grey::postExecute()
{
	return BVSStatus::OK;
}



BVSStatus grey::debugDisplay()
{
	return BVSStatus::OK;
}



// Executed at module unload.
BVSStatus grey::onClose()
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
		registerModule(id, new grey(id, config));

		return 0;
	}
}

