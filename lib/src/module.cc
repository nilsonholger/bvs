#include "bvs/module.h"
#include "loader.h"

using BVS::Module;


Module::Module()
{

}



Module::~Module()
{

}



void registerModule(const std::string& id, Module* module, bool hotSwap)
{
	BVS::Loader::registerModule(id, module, hotSwap);
}
