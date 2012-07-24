#include "blur.h"



// This is your module's constructor.
// Please do not change its signature as it is called by the framework (so the
// framework actually creates your module) and the framework assigns the unique
// identifier and gives you access to the its config.
// However, you should use it to create your data structures etc.
blur::blur(const std::string id, const BVS::Config& config)
	: BVS::Module()
	, id(id)
	, logger(id)
	, config(config)
	, input("input", BVS::ConnectorType::INPUT)
	, output("outBlur", BVS::ConnectorType::OUTPUT)
	, frame()
{

	cv::namedWindow("blur", 1);
}



// This is your module's destructor.
// See the constructor for more info.
blur::~blur()
{

}



// Put all your work here.
BVS::Status blur::execute()
{
	LOG(2, "Execution of " << id << "!");

	frame = input.get();
	//LOG(0, frame.total());
	if (frame.total() == 0) return BVS::Status::OK;
	cv::GaussianBlur(frame, frame, cv::Size(7,7), 1.5, 1.5);
	cv::imshow("blur", frame);
	//cv::imwrite("foo.bmp", frame);
	cv::waitKey(1);
	
	output.set() = frame;

	return BVS::Status::OK;
}



// UNUSED
BVS::Status blur::debugDisplay()
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
		registerModule(id, new blur(id, config));

		return 0;
	}
}

