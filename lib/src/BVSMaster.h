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
#include "BVSTraits.h"



/** The system master: loads, unloads and controls modules. */
class BVSMaster
{
    public:
        /** Constructor for master.
         * @param[in] bvsModuleMap Map of registered modules.
         * @param[in] config Reference to config system.
         */
        BVSMaster(BVSModuleMap& modules, BVSConfig& config);

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
        BVSMaster& control(std::shared_ptr<BVSModuleData> data);

        /** Get ID of module. */
        //TODO returns only first match, need to change model
        BVSModuleID getModuleID(std::string identifier);

    private:
        /** Map of registered modules and their metadata. */
        BVSModuleMap& modules;

        //std::condition_variable controller;
        //std::mutex threadMutex;

        BVSLogger logger; /**< Logger metadata. */
        BVSConfig& config; /**< Config reference. */

        BVSMaster(const BVSMaster&) = delete; /**< -Weffc++ */
        BVSMaster& operator=(const BVSMaster&) = delete; /**< -Weffc++ */
};



#endif //BVSMASTER_H

