#ifndef BVSMASTER_H
#define BVSMASTER_H

#include<cstdlib>
#include<iostream>
#include<list>
#include<map>
#include<string>
#include<vector>
#include<dlfcn.h>

#include "BVSConfig.h"
#include "BVSLogger.h"
#include "BVSModule.h"



/** The module loader. */
class BVSMaster
{
    public:
        /** Constructor for module loader.
         * @param[in] config Reference to config system.
         */
        BVSMaster(std::map<std::string, BVSModule*, std::less<std::string>>& bvsModuleMap, BVSConfig& config);

        /** Load the given module, executes bvsRegisterModule function in module
         * to register it with the system.
         * @param[in] moduleName THe name of the module.
         */
        BVSMaster& load(std::string moduleName);

        /** Unload the given module.
         * @param[in] moduleName The name of the module.
         */
        BVSMaster& unload(std::string moduleName);

    private:
        /** Map of registered Modules */
        std::map<std::string, BVSModule*, std::less<std::string>>& bvsModuleMap;

        /** Map of known/used/active library handles. */
        std::map<std::string, void*, std::less<std::string>> handleMap;

        BVSLogger logger; /**< Logger metadata. */
        BVSConfig& config; /**< Config reference. */

        BVSMaster(const BVSMaster&) = delete; /**< -Weffc++ */
        BVSMaster& operator=(const BVSMaster&) = delete; /**< -Weffc++ */
};



#endif //BVSMASTER_H

