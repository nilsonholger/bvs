#ifndef BVSMASTER_H
#define BVSMASTER_H

#include<cstdlib>
#include<condition_variable>
#include<iostream>
#include<list>
#include<map>
#include<string>
#include<thread>
#include<vector>
#include<dlfcn.h>

#include "BVSConfig.h"
#include "BVSLogger.h"
#include "BVSModule.h"



// Forward declaration.
struct BVSModuleData;



/** The system master: loads, unloads and controls modules. */
class BVSMaster
{
    public:
        /** Constructor for master.
         * @param[in] bvsModuleMap Map of registered modules.
         * @param[in] config Reference to config system.
         */
        BVSMaster(std::map<std::string, BVSModuleData*, std::less<std::string>>& bvsModuleMap, BVSConfig& config);

        /** Load the given module, executes bvsRegisterModule function in module
         * to register it with the system.
         * @param[in] moduleName The name of the module.
         * @param[in] asThread Whether to load the module inside a thread or not.
         */
        BVSMaster& load(const std::string& moduleName, bool asThread);

        /** Unload the given module.
         * @param[in] moduleName The name of the module.
         */
        BVSMaster& unload(const std::string& moduleName);

        // TODO add control functions for master and threads
        BVSMaster& control(BVSModuleData* data = nullptr);

    private:
        // TODO handle modules that are loaded more than once (unique names, module id's...)
        /** Map of registered modules and their metadata. */
        std::map<std::string, BVSModuleData*, std::less<std::string>>& bvsModuleMap;

        //std::condition_variable controller;
        //std::mutex threadMutex;

        BVSLogger logger; /**< Logger metadata. */
        BVSConfig& config; /**< Config reference. */

        BVSMaster(const BVSMaster&) = delete; /**< -Weffc++ */
        BVSMaster& operator=(const BVSMaster&) = delete; /**< -Weffc++ */
};



/** Module metadata. */
struct BVSModuleData
{
    std::string name; /**< Name of this module. */
    BVSModule* module; /**< Pointer to the module. */
    void* dlib; /**< Dlib handle to module's lib. */
    bool asThread; /**< Determines if module runs in its own thread. */
    std::thread thread; /**< Thread handle of module. */
};



#endif //BVSMASTER_H

