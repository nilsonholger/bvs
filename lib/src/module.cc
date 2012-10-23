#include "bvs/module.h"
#include "control.h"



BVS::Module::Module()
{

}



BVS::Module::~Module()
{

}



void registerModule(const std::string& id, BVS::Module* module, bool hotSwap)
{
	BVS::Control::registerModule(id, module, hotSwap);
}
