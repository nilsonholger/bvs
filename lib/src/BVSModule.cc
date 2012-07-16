#include "BVSModule.h"
#include "BVSLoader.h"



BVSModule::BVSModule()
{

}



BVSModule::~BVSModule()
{

}



void registerModule(const std::string& id, BVSModule* module)
{
	BVSLoader::registerModule(id, module);
}
