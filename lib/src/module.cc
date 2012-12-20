#include "bvs/module.h"
#include "loader.h"



void registerModule(const std::string& id, BVS::Module* module, bool hotSwap)
{
	BVS::Loader::registerModule(id, module, hotSwap);
}
