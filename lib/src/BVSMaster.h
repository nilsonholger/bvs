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



/** The system master, loads, unloads and controls modules. */
class BVSMaster
{
    public:
        /** Constructor for master.
         * @param[in] bvsModuleMap Map of registered modules.
         * @param[in] config Reference to config system.
         */
        BVSMaster(std::map<std::string, BVSModule*, std::less<std::string>>& bvsModuleMap, BVSConfig& config);

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

        void call_from_thread(BVSModule* module);

        // TODO NEXT add control functions for master and threads
        void masterController();
        void threadController(BVSModule* module);
        void threadJoinAll();

    private:
        /** Map of registered Modules */
        std::map<std::string, BVSModule*, std::less<std::string>>& bvsModuleMap;

        /** Map of known/used/active library handles. */
        std::map<std::string, void*, std::less<std::string>> handleMap;

        /** Vector of modules controlled directly by master. */
        std::vector<BVSModule*> masterModules;

        /** Vector of modules running inside own thread. */
        std::vector<std::thread> threadedModules;

        std::condition_variable controller;
        std::mutex threadMutex;

        BVSLogger logger; /**< Logger metadata. */
        BVSConfig& config; /**< Config reference. */

        BVSMaster(const BVSMaster&) = delete; /**< -Weffc++ */
        BVSMaster& operator=(const BVSMaster&) = delete; /**< -Weffc++ */
};



#endif //BVSMASTER_H

