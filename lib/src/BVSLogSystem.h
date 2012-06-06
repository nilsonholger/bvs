#ifndef BVSLOGSYSTEM_H
#define BVSLOGSYSTEM_H

#include<iomanip>
#include<memory>
#include<mutex>

#include "BVSConfig.h"
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

		/** Log to output. End this by calling endl() to release the log mutex.
		 * @param[in] logger Logger metadata from caller.
		 * @param[in] level The desired output verbosity of this message.
		 * @return Outstream to log to.
		 */
		std::ostream& out(const BVSLogger& logger, int level);

		/** Ends output log by appending std::endl and releasing the log mutex.
		*/
		void endl();

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
		BVSLogSystem& enableLogFile(const std::string& file, bool append = false);

		/** Close and disable log file.
		 * @return Reference to object.
		 */
		BVSLogSystem& disableLogFile();

		/** Enable log console/command line interface.
		 * @param out Stream to log to (default = std::cout).
		 * @return Reference to object.
		 */
		BVSLogSystem& enableLogConsole(const std::ostream& out = std::cout);

		/** Disable log console/command line interface.
		 * @return Reference to object.
		 */
		BVSLogSystem& disableLogConsole();

		/** Announces a logger instance to the backend.
		 * @param[in] logger Logger metadata from caller.
		 */
		BVSLogSystem& announce(const BVSLogger& logger);

		/** Check config for settings.
		 * Checks the given config object for occurences of:
		 * @code
		 * [BVS]
		 * logSystem = true/false
		 * logConsole = true/false
		 * logFile = $logFile # a '+' in front of the file name will append to the file
		 * @endcode
		 * @param[in] config Config object.
		 * @return Reference to object.
		 */
		BVSLogSystem& updateSettings(BVSConfig& config);

		/** Check config for client levels.
		 * Checks the given config object for occurences of BVSLogger.*.
		 * This way one can specify verbosity levels in a config file:
		 * @code
		 * [BVSLogger]
		 * ALL = 3 # set override for all loggers
		 * LoggerOne = 0
		 * LoggerTwo = 1
		 * @endcode
		 * @param[in] config Config object.
		 * @return Reference to object.
		 */
		BVSLogSystem& updateLoggerLevels(BVSConfig& config);

	private:
		/** Construct log system.
		*/
		BVSLogSystem();

		/** Logger clients' levels from config(s). */
		std::map<std::string, int, std::less<std::string>> loggerLevels;

		/** Name padding size for fancy output, updated in announce function. */
		unsigned int namePadding;

		/** The overall system verbosity level.
		 * The overall system verbosity level will be initialized to 3, only
		 * messages with a level below or equal this value will be logged.
		 * This value can be changed by using:
		 * @code
		 * [BVSLogger]
		 * All = 0 # your desired level
		 * @endcode
		 * */
		unsigned short systemVerbosity;

		static std::shared_ptr<BVSLogSystem> instance; /**< Logging system instance. */
		std::mutex outMutex; /**< Output mutex, needed for threaded scenarios. */

		static BVSNullStream nullStream; /**< Stream pointing to nirvana */
		std::ostream outCLI; /**< Stream pointing to Command Line Interface. */
		std::ofstream outFile; /**< Stream pointing to file. */
		BVSStreamDup outBoth; /**< Stream duplicator pointing to CLI and file. */

		BVSLogSystem(const BVSLogSystem&) = delete; /**< -Weffc++ */
		BVSLogSystem& operator=(const BVSLogSystem&) = delete; /**< -Weffc++ */
};

#endif //BVSLOGSYSTEM_H

