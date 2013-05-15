#ifndef BVS_TRAITS_H
#define BVS_TRAITS_H

#include <string>



/** Version information. */
static const unsigned int bvs_version_year = 2013;
static const unsigned int bvs_version_release = 3;
static const std::string bvs_version_nickname = "public-appearance";
static const std::string bvs_version = std::to_string(bvs_version_year)
	+ "." + std::to_string(bvs_version_release)
	+ "-" + bvs_version_nickname;



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
static const bool bvs_connector_type_matching = true;

/** Enable/disable the builtin logging system.
 * Also, all LOG(*) calls will be removed if disabled.
 *
 * Possible Values: true, false
 */
#ifdef BVS_LOG_SYSTEM
static const bool bvs_log_system = true;
#else
static const bool bvs_log_system = false;
#endif //BVS_LOG_SYSTEM

/** Set the logging system's maximum verbosity.
 * Only message of smaller levels will be logged.
 *
 * Possible (Meaningful) Values: 0, 1, 2, 3
 */
static const unsigned int bvs_log_system_verbosity = 3;

/** Whether to log to console or not.
 *
 * Possible Values: true, false
 */
static const bool bvs_log_to_console = true;

/** Whether to log to a file or not.
 * If the name is prepended with a '+' sign, instead of overwriting, append
 * to given file name.
 *
 * Possible Values: "" (NO log file), "$NAME", "+$NAME"
 */
static const std::string bvs_log_to_logfile = {};

/** The logging system clients' default verbosity.
 * This verbosity level will be used for all log clients that have not
 * explicitely set one.
 *
 * Possible Values: see bvs_log_system_verbosity
 */
static const unsigned int bvs_log_client_default_verbosity = 3;

/** Whether the system allows modules to run in threads.
 *
 * Possible Values: true, false
 */
static const bool bvs_module_threads = true;

/** Whether the system ENFORCES modules to run in threads.
 * NOTE: this does NOT disable module pools.
 *
 * Possible Values: true, false
 */
static const bool bvs_module_force_threads = false;

/** Whether the system allows module pools.
 *
 * Possible Values: true, false
 */
static const bool bvs_module_pools = true;



#ifdef BVS_GCC_VISIBILITY
#ifdef __GNUC__
#define BVS_PRIVATE __attribute__ ((visibility ("hidden")))
#define BVS_PUBLIC __attribute__ ((visibility ("default")))
#endif //__GNUC__
#else //BVS_GCC_VISIBILITY
/** @def BVS_PRIVATE
 * GCC visibility attribute macro for private/hidden objects.
 */
#define BVS_PRIVATE
/** @def BVS_PUBLIC
 * GCC visibility attribute macro for public objects.
 */
#define BVS_PUBLIC
#endif //BVS_GCC_VISIBILITY



#endif //BVS_TRAITS_H

