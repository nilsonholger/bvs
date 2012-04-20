#ifndef BVSMODULELOADER_H
#define BVSMODULELOADER_H

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
class BVSModuleLoader
{
    public:
        /** Constructor for module loader.
         * @param[in] config Reference to config system.
         */
        BVSModuleLoader(BVSConfig& config);

        /** Load the given module, executes addBVSModule function in module to register it.
         * @param path Path to module.
         */
        void load(std::string path);

    private:
        BVSLogger logger; /**< Logger metadata. */
        BVSConfig& config; /**< Config reference. */
        BVSModuleLoader(const BVSModuleLoader&) = delete; /**< -Weffc++ */
        BVSModuleLoader& operator=(const BVSModuleLoader&) = delete; /**< -Weffc++ */
};



#endif //BVSMODULELOADER_H

