#include "BVSMaster.h"



BVSMaster::BVSMaster(std::map<std::string, BVSModule*, std::less<std::string>>& bvsModuleMap, BVSConfig& config)
    : bvsModuleMap(bvsModuleMap)
    , handleMap()
    , logger("BVSMaster")
    , config(config)
{

}



BVSMaster& BVSMaster::load(const std::string& moduleName)
{
    // prepare path and load the lib
    std::string modulePath = "./lib" + moduleName + ".so";
    LOG(3, moduleName << " will be loaded from " << modulePath);
    void* dlib = dlopen(modulePath.c_str(), RTLD_NOW);

    // check for errors
    if (dlib == NULL)
    {
        LOG(0, "While loading " << modulePath << ", following error occured: " << dlerror());
        exit(-1);
    }

    // look for bvsRegisterModule in loaded lib, check for errors and execute register function
    typedef void (*registerModule_t)(BVSConfig& config);
    registerModule_t registerModule;
    *reinterpret_cast<void**>(&registerModule)=dlsym(dlib, "bvsRegisterModule");

    // check for errors
    char* dlerr = dlerror();
    if (dlerr)
    {
        LOG(0, "Loading function bvsRegisterModule() in " << modulePath << " resulted in: " << dlerr);
        exit(-1);
    }
    registerModule(config);
    LOG(2, moduleName << " loaded and registered!");

    // save handle for later use
    handleMap[moduleName] = dlib;

    return *this;
}



BVSMaster& BVSMaster::unload(const std::string& moduleName)
{
    // close lib and check for errors
    std::string modulePath = "./lib" + moduleName + ".so";
    LOG(3, moduleName << " will be closed using " << modulePath);

    // get handle from internals
    void* dlib = handleMap[moduleName];
    if (dlib==nullptr)
    {
        LOG(0, "Requested module " << moduleName << " not found!");
        exit(-1);
    }

    // close the module
    dlclose(dlib);

    // check for errors
    char* dlerr = dlerror();
    if (dlerr)
    {
        LOG(0, "While closing " << modulePath << " following error occured: " << dlerror());
        exit(-1);
    }

    // deregister module from system
    bvsModuleMap.erase(moduleName);
    LOG(2, moduleName << " unloaded and deregistered!");

    // remove handle
    handleMap.erase(moduleName);

    return *this;
}
