#include "bvs/module.h"
#include "loader.h"



BVS::Module::Module()
{

}



BVS::Module::~Module()
{

}



void registerModule(const std::string& id, BVS::Module* module, bool hotSwap)
{
	BVS::Loader::registerModule(id, module, hotSwap);
}
