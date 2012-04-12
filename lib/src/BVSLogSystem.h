#ifndef BVSLOGSYSTEM_H
#define BVSLOGSYSTEM_H

#include<iomanip>
#include<memory>

#include "BVSStreams.h"
#include "BVSLogger.h"



/** A logging system using std::ostream.
 * This creates a logging mechanism. It builds the backend for BVSLogger. Each
 * BVSLogger instance shares the output streams with all other instances.
 * This will get automagically instantiated, once a BVSLogger is created.
 *
 * The logging levels loosely represent:
 * 0 ERROR
 * 1 WARNING
 * 2 INFO
 * 3 DEBUG
 * 4 and beyond are CUSTOM logging levels.
 */
class BVSLogSystem
{
    public:
        /** Connect to logging System.
         * @return Pointer to logging instance.
         */
        static std::shared_ptr<BVSLogSystem> connectToLogSystem();

        /** Log to output.
         * @param[in] logger Logger metadata from caller.
         * @param[in] level The desired output verbosity of this message.
         * @return Outstream to log to.
         */
        std::ostream& out(const BVSLogger& logger, int level);

        /** Set the system verbosity level.
         * @param[in] verbosity The desired verbosity level.
         * @return Reference to object.
         */
        BVSLogSystem& setSystemVerbosity(const int verbosity);

        /** Open and enable log file.
         * Open and enable log file at given file location.
         * Furthermore you can decide to overwrite (default) or append to the file.
         * @param[in] file Path to file to log to.
         * @param[in] append Select, whether to append or overwrite.
         * @return Reference to object.
         */
        BVSLogSystem& enableLogFile(std::string file, bool append = false);

        /** Close and disable log file.
         * @return Reference to object.
         */
        BVSLogSystem& disableLogFile();

        /** Enable log console/command line interface.
         * @param out Stream to log to (default = std::cout).
         * @return Reference to object.
         */
        BVSLogSystem& enableLogConsole(std::ostream& out = std::cout);

        /** Disable log console/command line interface.
         * @return Reference to object.
         */
        BVSLogSystem& disableLogConsole();

        /** Announces a logger instance to the backend.
         * @param[in] logger Logger metadata from caller.
         */
        BVSLogSystem& announce(const BVSLogger& logger);

    private:
        /** Construct log system.
         */
        BVSLogSystem();

        unsigned int namePadding; /**< Name padding size for fancy output, updated in announce function. */
        unsigned short systemVerbosity; /**< The overall system verbosity level. */
        static std::shared_ptr<BVSLogSystem> instance; /**< Logging system instance. */
        static BVSNullStream nullStream; /**< Stream pointing to nirvana */
        std::ostream outCLI; /**< Stream pointing to Command Line Interface. */
        std::ofstream outFile; /**< Stream pointing to file. */
        BVSStreamDup outBoth; /**< Stream duplicator pointing to CLI and file. */

        BVSLogSystem(const BVSLogSystem&) = delete; /**< -Weffc++ */
        BVSLogSystem& operator=(const BVSLogSystem&) = delete; /**< -Weffc++ */
};

#endif //BVSLOGSYSTEM_H

