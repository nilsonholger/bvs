#include "BVSModuleLoader.h"



BVSModuleLoader::BVSModuleLoader(BVSConfig& config)
    : logger("BVS::Loader")
    , config(config)
{

}



void BVSModuleLoader::load(std::string moduleName)
{
    // load the lib and check for errors
    std::string modulePath = "./lib" + moduleName + ".so";
    LOG(2, moduleName << " will be loaded from " << modulePath);
    LOG(2, modulePath << " loading!");
    void* dlib = dlopen(modulePath.c_str(), RTLD_NOW);
    if (dlib == NULL)
    {
        LOG(0, "While loading " << modulePath << ", following error occured: " << dlerror());
        exit(-1);
    }

    // look for bvsAddModule in loaded lib, check for errors and execute register function
    typedef void (*registerModule_t)(BVSConfig& config);
    registerModule_t registerModule;
    *reinterpret_cast<void**>(&registerModule)=dlsym(dlib, "bvsRegisterModule");
    char* dlerr = dlerror();
    if (dlerr)
    {
        LOG(0, "Loading function bvsRegisterModule() in " << modulePath << " resulted in: " << dlerr);
        exit(-1);
    }
    registerModule(config);
    LOG(2, modulePath << " loaded and registered!");
}
