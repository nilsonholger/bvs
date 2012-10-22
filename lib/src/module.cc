#include "bvs/module.h"
#include "control.h"



BVS::Module::Module()
{

}



BVS::Module::~Module()
{

}



BVS::Status BVS::Module::prepareHotSwap(void*&)
{
	return BVS::Status::OK;
}



void registerModule(const std::string& id, BVS::Module* module, bool hotSwap)
{
	BVS::Control::registerModule(id, module, hotSwap);
}
