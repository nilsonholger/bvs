#ifndef BVS_H
#define BVS_H

#include<iomanip>
#include<iostream>
#include<map>
#include<string>

#include "BVSConfig.h"
#include "BVSLogger.h"



class BVSConfig;
class BVSModule;
class BVSModuleLoader;



/** The BVS framework base.
 * This is the BVS base, use this to interact with the framework.
 */
class BVS
{
    public:
        /** Create BVS System.
         * @param[in] argc Main's argc.
         * @param[in] argv Main's argv.
         */
        BVS(int argc, char** argv);

        /** Load modules selected by config variable modules.
         */
        BVS& loadModules();

        // TODO create BVSControler
        // TODO add and use options to configure BVS
        //void start();
        //void step();
        //void pause();
        //void stop();
        //void close();
        //~BVS();

        /** Set the log system verbosity.
         * This sets the logging system's overall verbosity.
         * Only messages with logging level lower or equal with be displayed.
         * @param[in] verbosity Desired verbosity level.
         * @return Reference to object.
         */
        BVS& setLogSystemVerbosity(unsigned short verbosity);

        /** Open and enable log file.
         * Open and enable log file at given file location.
         * Furthermore you can decide to overwrite (default) or append to the file.
         * @param[in] file Path to file to log to.
         * @param[in] append Select, whether to append or overwrite.
         * @return Reference to object.
         */
        BVS& enableLogFile(std::string file, bool append = false);

        /** Close and disable log file.
         * @return Reference to object.
         */
        BVS& disableLogFile();

        /** Enable log console/command line interface.
         * @param[out] out Stream to log to (default = std::cout).
         * @return Reference to object.
         */
        BVS& enableLogConsole(std::ostream& out = std::cout);

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
        static void registerModule(std::string identifier, BVSModule* module);

    private:
        std::shared_ptr<BVSLogSystem> logSystem; /**< Internal log system backend. */
        BVSLogger logger; /**< BVS' logging instance. */

    public:
        BVSConfig config; /**< BVS' config system. */

    private:
        BVSModuleLoader* loader; /**< BVS' module loader. */

        BVS(const BVS&) = delete; /**< -Weffc++ */
        BVS& operator=(const BVS&) = delete; /**< -Weffc++ */
        /** Map op modules known by the BVS framework. */
        static std::map<std::string, BVSModule*, std::less<std::string>> bvsModuleMap;
};



#endif //BVS_H

