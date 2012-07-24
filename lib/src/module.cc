#include "bvs/module.h"
#include "loader.h"



BVS::Module::Module()
{

}



BVS::Module::~Module()
{

}



void registerModule(const std::string& id, BVS::Module* module)
{
	BVS::Loader::registerModule(id, module);
}
