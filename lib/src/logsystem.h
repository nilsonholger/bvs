#ifndef BVS_LOGSYSTEM_H
#define BVS_LOGSYSTEM_H

#include <fstream>
#include <functional>
#include <memory>
#include <mutex>

#include "bvs/config.h"
#include "streams.h"
#include "bvs/logger.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** A logging system using std::ostream.
	 * This creates a logging mechanism. It builds the backend for Logger. Each
	 * Logger instance shares the output streams with all other instances.
	 * This will get automagically instantiated, once a Logger is created.
	 *
	 * The logging levels loosely represent:
	 * 0 ERROR
	 * 1 WARNING
	 * 2 INFO
	 * 3 DEBUG
	 * 4 and beyond are CUSTOM logging levels.
	 */
	class LogSystem
	{
		public:
			/** Connect to logging System.
			 * @return Pointer to logging instance.
			 */
			static std::shared_ptr<LogSystem> connectToLogSystem();

			/** Log to output. End this by calling endl() to release the log mutex.
			 * @param[in] logger Logger metadata from caller.
			 * @param[in] level The desired output verbosity of this message.
			 * @return Outstream to log to.
			 */
			std::ostream& out(const Logger& logger, int level);

			/** Ends output log by appending std::endl and releasing the log mutex.
			 * @param[in] logger Logger metadata from caller.
			 * @param[in] level The desired output verbosity of this message.
			*/
			void endl(const Logger& logger, const int level);

			/** Set the system verbosity level.
			 * @param[in] verbosity The desired verbosity level.
			 * @return Reference to object.
			 */
			LogSystem& setSystemVerbosity(const int verbosity);

			/** Open and enable log file.
			 * Open and enable log file at given file location.
			 * Furthermore you can decide to overwrite (default) or append to the file.
			 * @param[in] file Path to file to log to.
			 * @param[in] append Select, whether to append or overwrite.
			 * @return Reference to object.
			 */
			LogSystem& enableLogFile(const std::string& file, bool append = false);

			/** Close and disable log file.
			 * @return Reference to object.
			 */
			LogSystem& disableLogFile();

			/** Enable log console/command line interface.
			 * @param out Stream to log to (default = std::cout).
			 * @return Reference to object.
			 */
			LogSystem& enableLogConsole(const std::ostream& out = std::cout);

			/** Disable log console/command line interface.
			 * @return Reference to object.
			 */
			LogSystem& disableLogConsole();

			/** Announces a logger instance to the backend.
			 * @param[in] logger Logger metadata from caller.
			 */
			LogSystem& announce(const Logger& logger);

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
			LogSystem& updateSettings(const Config& config);

			/** Check config for client levels.
			 * Checks the given config object for occurences of Logger.*.
			 * This way one can specify verbosity levels in a config file:
			 * @code
			 * [Logger]
			 * LoggerOne = 0
			 * LoggerTwo = 1
			 * @endcode
			 * @param[in] config Config object.
			 * @return Reference to object.
			 */
			LogSystem& updateLoggerLevels(const Config& config);

		private:
			/** Construct log system.
			*/
			LogSystem();

			/** Logger clients' levels from config(s) in lowercase. */
			std::map<std::string, int, std::less<std::string>> loggerLevels;

			/** Temp object needed for output and announce function. */
			std::string tmpName;

			/** Name padding size for fancy output, updated in announce function. */
			unsigned int namePadding;

			/** Whether to output colors in the log output.
			 * Messages to level 0 (ERROR) are red, messages to level 1 (INFO)
			 * are yellow.
			 */
			bool logColors;

			/** The overall system verbosity level.
			 * The overall system verbosity level will be initialized to 3, only
			 * messages with a level below or equal this value will be logged.
			 * This value can be changed by using:
			 * @code
			 * [BVS]
			 * logVerbosity = 0 # your desired level
			 * @endcode
			 * */
			unsigned short systemVerbosity;

			static std::shared_ptr<LogSystem> instance; /**< Logging system instance. */
			std::mutex outMutex; /**< Output mutex, needed for threaded scenarios. */

			static NullStream nullStream; /**< Stream pointing to nirvana */
			std::ostream outCLI; /**< Stream pointing to Command Line Interface. */
			std::ofstream outFile; /**< Stream pointing to file. */
			StreamDup outBoth; /**< Stream duplicator pointing to CLI and file. */

			LogSystem(const LogSystem&) = delete; /**< -Weffc++ */
			LogSystem& operator=(const LogSystem&) = delete; /**< -Weffc++ */
	};
} // namespace BVS



#endif //BVS_LOGSYSTEM_H

