#ifndef BVS_LOGGER_H
#define BVS_LOGGER_H

#include<iostream>
#include<memory>
#include<string>



/** Macro to use with Logger. */
#ifdef BVS_LOG_SYSTEM
#define LOG(level, args) { logger.out(level) << args << std::endl; logger.endl(); };
#else
#define LOG(level, args) { };
#endif



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	// Forward declaration.
	class LogSystem;



	/** Provides access to the LogSystem.
	 * To use this system, just create a Logger object with your desired
	 * settings. The LogSystem backend will be initialized automatically.
	 * Please call your Logger instance "logger", that way the above defined
	 * macro can be used (which SHOULD be used, thus allowing for a complete
	 * disabling of all logging activity requiring zero overhead).
	 */
	class Logger
	{
		public:
			/** Available logging targets. */
			enum LogTarget { OFF, TO_CLI, TO_FILE, TO_CLI_AND_FILE};

			/** Construct logger metadata.
			 * Your logging instances' name will be prepended to your output.
			 * Since this output will be aligned, please dont use too long names.
			 * Example:
			 * <code>
			 * [0|YourLogger   ] message from you
			 * [1|AnotherLogger] message
			 * </code>
			 * @param[in] name The name of your logging instance.
			 * @param[in] verbosity Your selected logging verbosity level (default: 3).
			 * @param[in] target Selects this loggers output target (default: TO_CLI_AND_FILE)
			 */
			Logger(std::string name, unsigned short verbosity = 3, LogTarget target = TO_CLI_AND_FILE);

			/** Log to logging system.
			 * @param[in] level The messages' desired verbosity level.
			 * @return A stream reference your output will be send to.
			 */
			std::ostream& out(const int level);

			/* Ends a log line and releases the logSystem mutex, must be called after using out.
			*/
			void endl();

			/** Get this logger's name.
			 * @return This logger's name.
			 */
			std::string getName() const;

			~Logger(); /**< Deconstructor cleaning up. */

			unsigned short verbosity; /**< This logger's verbosity level. */
			LogTarget target; /**< This logger's output target. */

		private:
			/** This logger instance's name.
			 * Private to prevent changes later on, which would
			 * mess with the name padding in the logging system.
			 * */
			std::string name;

#ifdef BVS_LOG_SYSTEM
			std::shared_ptr<LogSystem> logSystem; /**< Pointer to the logging backend. */
#endif

			Logger(const Logger&) = delete; /**< -Weffc++ */
			Logger& operator=(const Logger&) = delete; /**< -Weffc++ */
	};
} // namespace BVS



#endif //BVS_LOGGER_H

