#include "canny.h"



// This is your module's constructor.
// Please do not change its signature as it is called by the framework (so the
// framework actually creates your module) and the framework assigns the unique
// identifier and gives you access to the its config.
// However, you should use it to create your data structures etc.
canny::canny(const std::string id, const BVS::Config& config)
	: BVS::Module()
	, id(id)
	, logger(id)
	, config(config)
	, input("input", BVS::ConnectorType::INPUT)
	, output("cannyOut", BVS::ConnectorType::OUTPUT)
	, frame()
	, start(std::chrono::high_resolution_clock::now())
	, counter(0)
{

	cv::namedWindow("canny", 1);
}



// This is your module's destructor.
// See the constructor for more info.
canny::~canny()
{

}



// Put all your work here.
BVS::Status canny::execute()
{
	LOG(2, "Execution of " << id << "!");

	frame = input.get();
	//LOG(0, frame.total());
	if (frame.total() == 0) return BVS::Status::OK;
	cv::Canny(frame, frame, 0, 30, 3);
	cv::imshow("canny", frame);
	//cv::imwrite("foo.bmp", frame);
	cv::waitKey(1);
	
	output.set() = frame;

	counter++;
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-start).count() > 1000)
	{
		LOG(0, "fps: " << counter);
		start = std::chrono::high_resolution_clock::now();
		counter = 0;
	}

	return BVS::Status::OK;
}



// UNUSED
BVS::Status canny::debugDisplay()
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
		registerModule(id, new canny(id, config));

		return 0;
	}
}

