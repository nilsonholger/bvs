#include "BVSModuleLoader.h"



BVSModuleLoader::BVSModuleLoader()
    : logger("BVS::Loader")
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
    typedef void (*addModule_t)();
    addModule_t addModule;
    *reinterpret_cast<void**>(&addModule)=dlsym(dlib, "bvsAddModule");
    char* dlerr = dlerror();
    if (dlerr)
    {
        LOG(0, "Calling bvsAddModule() in " << modulePath << " resulted in: " << dlerr);
        exit(-1);
    }
    addModule();
    LOG(2, modulePath << " loaded and registered!");
}

