#ifndef BVSLOGGER_H
#define BVSLOGGER_H

#include<fstream>
#include<iostream>
#include<map>
#include<memory>
#include<streambuf>
#include<string>



/** Macro to use with BVSLogger. */
#ifndef BVS_LOG_DISABLED
#define LOG(level, args) logger.out(level) << args << std::endl
#else
#define LOG(level, args)
#endif



class BVSLogSystem; /**< Forward declaration needed for BVSLogger. */



/** The BVS Logging mechanism.
 * To use this system, just create a BVSLogger object with your desired
 * settings. The BVSLogSystem backend will be initialized automatically.
 * Please call your BVSLogger instance "logger", that way the above defined
 * macro can be used (which SHOULD be used, thus allowing for a complete
 * disabling of all logging activity requiring zero overhead).
 */
class BVSLogger
{
    public:
        /** Available logging targets. */
        enum BVSLogTarget { OFF, TO_CLI, TO_FILE, TO_CLI_AND_FILE};

        /** Construct logger metadata.
         * Your logging instances' name will be prepended to your output.
         * Since this output will be aligned, please dont use too long names.
         * Example:
         * <code>
         * [0|   YourLogger] message from you
         * [1|AnotherLogger] message
         * </code>
         * @param[in] name The name of your logging instance.
         * @param[in] verbosity Your selected logging verbosity level (default: 3).
         * @param[in] target Selects this loggers output target (default: TO_CLI_AND_FILE)
         */
        BVSLogger(std::string name, unsigned short verbosity = 3, BVSLogTarget target = TO_CLI_AND_FILE);

        /** Log to logging system.
         * @param[in] level The messages' desired verbosity level.
         * @return A stream reference your output will be send to.
         */
        std::ostream& out(int level);

        /** Get this logger's name.
         * @return This logger's name.
         */
        std::string getName() const;

        ~BVSLogger(); /**< Deconstructor cleaning up. */

        unsigned short verbosity; /**< This logger's verbosity level. */
        BVSLogTarget target; /**< This logger's output target. */

    private:
        /** This logger instance's name.
         * Private to prevent changes later on, which would
         * mess with the name padding in the logging system.
         * */
        std::string name;
        std::shared_ptr<BVSLogSystem> logSystem; /**< Pointer to the logging backend. */

        BVSLogger(const BVSLogger&) = delete; /**< -Weffc++ */
        BVSLogger& operator=(const BVSLogger&) = delete; /**< -Weffc++ */
};

#endif //BVSLOGGER_H

