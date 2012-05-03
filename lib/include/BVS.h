#ifndef BVS_H
#define BVS_H

#include<iomanip>
#include<iostream>
#include<map>
#include<string>

#include "BVSConfig.h"
#include "BVSLogger.h"



// Forward declarations
class BVSModule;
class BVSMaster;



/** The BVS framework base.
 * This is the BVS base, use this to interact with the framework.
 * TODO add comments for BVS settings... and maybe usage
 */
class BVS
{
    public:
        /** Create BVS System.
         * @param[in] argc Main's argc.
         * @param[in] argv Main's argv, used to pass config options to BVS, see BVSConfig.
         */
        BVS(int argc, char** argv);

        /** Load modules selected by config variable [BVS]modules.
         */
        BVS& loadModules();

        /** Loads a config File and updates the system.
         * @param[in] configFile Name of or path to config file.
         * @return Refenrence to object.
         */
        BVS& loadConfigFile(const std::string& configFile);

        /** Set the log system verbosity.
         * This sets the logging system's overall verbosity.
         * Only messages with logging level lower or equal with be displayed.
         * Attention: This overrides the BVSLog.ALL setting from config file.
         * @param[in] verbosity Desired verbosity level.
         * @return Reference to object.
         */
        BVS& setLogSystemVerbosity(const unsigned short verbosity);

        /** Open and enable log file.
         * Open and enable log file at given file location.
         * Furthermore you can decide to overwrite (default) or append to the file.
         * @param[in] file Path to file to log to.
         * @param[in] append Select, whether to append or overwrite.
         * @return Reference to object.
         */
        BVS& enableLogFile(const std::string& file, bool append = false);

        /** Close and disable log file.
         * @return Reference to object.
         */
        BVS& disableLogFile();

        /** Enable log console/command line interface.
         * @param[out] out Stream to log to (default = std::cout).
         * @return Reference to object.
         */
        BVS& enableLogConsole(const std::ostream& out = std::cout);

        /** Disable log console/command line interface.
         * @return Reference to object.
         */
        BVS& disableLogConsole();

        /** Register a Module with the BVS framework.
         * This registers a module and should be used as a callback by the module
         * itself passing its name and a pointer to itself.
         * @param[in] identifier The Module's name.
         * @param[in] module A pointer to the module.
         */
        static void registerModule(const std::string& identifier, BVSModule* module);

        // TODO add threading, use to test concurrency issues throughout the system
        // TODO mark in BVSConfig as threaded by using '+' pre/postfix
        // TODO build data exchange between modules
        // TODO above options should be set in config (logTarget, file...)
        BVS& run();
        //void step();
        //void pause();
        //void stop();
        //void close();
        //~BVS();

        BVSConfig config; /**< BVS' config system. */

    private:
        std::shared_ptr<BVSLogSystem> logSystem; /**< Internal log system backend. */
        BVSLogger logger; /**< BVS' logging instance. */
        BVSMaster* master; /**< BVS' module loader. */

        /** Map of modules known by the BVS framework. */
        static std::map<std::string, BVSModule*, std::less<std::string>> bvsModuleMap;

        BVS(const BVS&) = delete; /**< -Weffc++ */
        BVS& operator=(const BVS&) = delete; /**< -Weffc++ */
};



#endif //BVS_H

