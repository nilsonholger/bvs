#include "grey.h"



// This is your module's constructor.
// Please do not change its signature as it is called by the framework (so the
// framework actually creates your module) and the framework assigns the unique
// identifier and gives you access to the its config.
// However, you might use it to create your data structures etc., or you can use
// the onLoad() and onClose() functions, just be consistent in order to avoid
// weird errors.
grey::grey(const std::string id, const BVS::Config& config)
	: BVS::Module()
	, id(id)
	, logger(id)
	, config(config)
	, input("input", BVS::ConnectorType::INPUT)
	//, output("testOut", BVS::ConnectorType::OUT)
	, frame()
{

}



// This is your module's destructor.
// See the constructor for more info.
grey::~grey()
{

}



// Put all your work here.
BVS::Status grey::execute()
{
	LOG(2, "Execution of " << id << "!");

	frame = input.get();
	LOG(0, frame.total());
	if (frame.total() == 0) return BVS::Status::OK;
	//cv::cvtColor(frame, frame, CV_BGR2GRAY);
	cv::namedWindow("grey", 1);
	cv::imshow("grey", frame);
	//cv::imwrite("foo.bmp", frame);

	return BVS::Status::OK;
}



BVS::Status grey::debugDisplay()
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
		registerModule(id, new grey(id, config));

		return 0;
	}
}

