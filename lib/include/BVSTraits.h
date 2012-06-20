#ifndef BVSTRAITS_H
#define BVSTRAITS_H



/** Enable/disable the builtin logging system.
 * Also, all LOG(*) calls will be removed if disabled.
 * Possible Values: true, false
 */
#define BVS_LOG_SYSTEM true

/** Set the logging system's maximum verbosity.
 * Only message of smaller levels will be logged.
 * Possible (Meaningful) Values: 0, 1, 2, 3
 */
#define BVS_LOG_SYSTEM_VERBOSITY 3

/** Whether to log to console or not.
 * Possible Values: true, false
 */
#define BVS_LOG_TO_CONSOLE true

/** Whether to log to a file or not.
 * If the name is prepended with a '+' sign, instead of overwriting, append
 * to given file name.
 * Possible Values: "" (NO log file), "$NAME"
 */
#define BVS_LOG_TO_LOGFILE ""

/** The logging system clients' default verbosity.
 * This verbosity level will be used for all log clients that have not
 * explicitely set one.
 * Possible Values: see BVS_LOG_SYSTEM_VERBOSITY
 */
#define BVS_LOG_CLIENT_DEFAULT_VERBOSITY 3

/** Whether the system allows modules to run in threads.
 * Possible Values: true, false
 */
#define BVS_MODULE_THREADS true

/** Whether the system ENFORCES modules to run in threads.
 * Possible Values: true, false
 */
#define BVS_MODULE_FORCE_THREADS false



#endif //BVSTRAITS_H

