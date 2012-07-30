#ifndef BVS_TRAITS_H
#define BVS_TRAITS_H



/** Enable/disable matching of connector types.
 * If enabled, the system tries to check if associated connectors are of
 * the same type, although it is completely type agnostic to that regard.
 * While it uses C++11's std::type_info::hash_code for that, it might
 * eventually be necessary to disable this if you are trying to use
 * modules that were compiled by a different compiler or on a different
 * architecture than your own.
 *
 * Possible Values: true, false
 */
#define BVS_CONNECTOR_TYPE_MATCHING true

/** Enable/disable the builtin logging system.
 * Also, all LOG(*) calls will be removed if disabled.
 *
 * Possible Values: true, false
 */
#define BVS_LOG_SYSTEM true

/** Set the logging system's maximum verbosity.
 * Only message of smaller levels will be logged.
 *
 * Possible (Meaningful) Values: 0, 1, 2, 3
 */
#define BVS_LOG_SYSTEM_VERBOSITY 3

/** Whether to log to console or not.
 *
 * Possible Values: true, false
 */
#define BVS_LOG_TO_CONSOLE true

/** Whether to log to a file or not.
 * If the name is prepended with a '+' sign, instead of overwriting, append
 * to given file name.
 *
 * Possible Values: "" (NO log file), "$NAME", "+$NAME"
 */
#define BVS_LOG_TO_LOGFILE ""

/** The logging system clients' default verbosity.
 * This verbosity level will be used for all log clients that have not
 * explicitely set one.
 *
 * Possible Values: see ::BVS_LOG_SYSTEM_VERBOSITY
 */
#define BVS_LOG_CLIENT_DEFAULT_VERBOSITY 3

/** Whether the system allows modules to run in threads.
 *
 * Possible Values: true, false
 */
#define BVS_MODULE_THREADS true

/** Whether the system ENFORCES modules to run in threads.
 *
 * Possible Values: true, false
 */
#define BVS_MODULE_FORCE_THREADS false



#endif //BVS_TRAITS_H

