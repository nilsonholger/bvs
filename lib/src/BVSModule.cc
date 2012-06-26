#include "BVSModule.h"
#include "BVSLoader.h"



BVSModule::BVSModule(const BVSConfig& config)
	: config(config)
{

}



BVSModule::~BVSModule()
{

}



void registerModule(const std::string& identifier, BVSModule* module)
{
	BVSLoader::registerModule(identifier, module);
}
