#ifndef BVS_H
#define BVS_H

#include <functional>
#include <iostream>
#include <string>

#include "bvs/config.h"
#include "bvs/connector.h"
#include "bvs/info.h"
#include "bvs/logger.h"
#include "bvs/traits.h"



/** @mainpage The BVS framework.
 * Here be dragons ;-)
 */



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	// Forward declarations
	class Loader;
	class Control;



	/** The BVS framework base.
	 * This is the BVS base, use this to interact with the framework.
	 *
	 * Usage:
	 * @li Create BVS object passing argc and argv if you want to use it.
	 * @li Call desired settings, e.g. enableLogFile(), enableLogConsole() etc.
	 * @li Call loadModules() to load all modules specified in settings or on command line.
	 * @li Call connectAllModules() to connect loaded modules.
	 * @li Call start() to prepare the controller (e.g. fork it).
	 * @li Call desired control functions, e.g. run(), step(), pause() etc.
	 * @li Call quit() to shutdown system.
	 *
	 * Config options (must belong to section [BVS], see example config/traits header for more information):
	 * @li \c connectorTypeMatching warns about connector type mismatch (ON/OFF).
	 * @li \c logSystem enables the logging system (ON/OFF).
	 * @li \c logConsole enables console output (ON/OFF).
	 * @li \c logFile enables logging to file (""/$FILE/+$FILE, '+' appends).
	 * @li \c logVerbosity sets the overall log verbosity (0/1/2/3...).
	 * @li \c moduleThreads allows modules to run in dedicated threads (ON/OFF).
	 * @li \c forceModuleThreads forces modules to run in dedicated threads (ON/OFF).
	 * @li \c modulePools allows modules to be bundled in a pool thread (ON/OFF).
	 * @li \c modules lists modules to load and their options.
	 *
	 * Module Syntax:
	 * @code
	 * # modules [+]= [+|poolName]id[(library[.configuration])][.connectorOptions]
	 * #
	 * # [+] -> append the module list
	 * # [+|[poolName]] -> Load module inside its own pool ('+') or add/create to a
	 * #             module pool of name '[poolName]' which also runs inside its own
	 * #             thread and executes added modules in the given order.
	 * #             '+' is effectively a shorthand for '[id]id'.
	 * # [(library...)] -> use as module library, useful more multiple modules from
	 * #                   one library
	 * # [.configuration] -> use this configuration for the module, useful so the
	 * #                     module name does not change but its configuration does
	 * # [.connectorOptions] -> options for connectors, look as follows:
	 * #                        input(test.output)[.input2(test.output2)]...
	 * #
	 * # if configuration and/or library are not given, the system will use the given
	 * # id instead
	 * #
	 * # examples:
	 * modules = id(library)
	 * modules += +id2(library2).input(id.output)
	 * modules += +id3(library2.configuration).input(id2.output)
	 * modules += [pool]id4.input(id3.output)
	 * @endcode
	 *
	 * To set the verbosity level of different/any logger instance (belongs to section [Logger]):
	 * @code
	 * [Logger]
	 * BVS = 0
	 * Daemon = 0
	 * Master = 0
	 * YourLogger = ...
	 * @endcode
	 *
	 * If you want to override configuration options on the command line, make sure
	 * to pass argc and argv to BVS. Then you can override command line options with:
	 * @code
	 * --bvs.options=section.option1=value1:section.option2=value2
	 * @endcode
	 * For more information, please see Config (note that $NAME in this case will be
	 * 'bvs').
	 */
	class BVS_PUBLIC BVS
	{
		public:
			/** Create BVS System.
			 * @param[in] argc Main's argc.
			 * @param[in] argv Main's argv, used to pass config options to BVS, see Config.
			 * @param[in] shutdownHandler A function the framework calls upon shutting down.
			 */
			BVS(int argc, char** argv, std::function<void()> shutdownHandler = [](){ exit(0);} );

			/** Destructor.
			 */
			~BVS();

			/** Load modules selected by config variable [BVS]modules.
			 * @return Reference to object.
			 */
			BVS& loadModules();

			/** Load selected module given by name.
			 * @param[in] moduleTraits The module id, library name and connector settings, formatted as: "id(library).connector(source.connector)..."
			 * @param[in] singlePool Select, if the module should run in it's own pool(thread).
			 * @param[in] poolName Select, if desired, the module pool to execute this module.
			 * @return Reference to object.
			 */
			BVS& loadModule(const std::string& moduleTraits, bool singlePool = false, std::string poolName = std::string());

			/** Unload all modules.
			 * @return Reference to object.
			 */
			BVS& unloadModules();

			/** Unload module given by name.
			 * @param[in] id The name of the module.
			 * @return Reference to object.
			 */
			BVS& unloadModule(const std::string& id);

			/** Connects module connectors.
			 * This connects ALL known module connectors by walking through the
			 * list of known modules and scanning their supplied options.
			 * @return Reference to object.
			 */
			BVS& connectAllModules();

			/** Connects selected module.
			 * This connects the selected module.
			 * @return Reference to object.
			 */
			BVS& connectModule(const std::string id);

			/** Loads a config File and updates the system.
			 * @param[in] configFile Name of or path to config file.
			 * @return Reference to object.
			 */
			BVS& loadConfigFile(const std::string& configFile);

			/** Set the log system verbosity.
			 * This sets the logging system's overall verbosity.
			 * Only messages with logging level lower or equal with be displayed.
			 * Attention: This overrides the BVSLOGGER.ALL setting from config file.
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

			/** Starts the system.
			 * This starts the system, but does not actually *start* the system.
			 * It prepares the masterController and makes sure that all threads
			 * are in a state where they can be controlled by the master.
			 *
			 * ATTENTION: When NOT forking the master controller and calling
			 * BVSControl::run(), your thread of execution will never return,
			 * because it stays inside the frameworks master control function.
			 * @see BVSControl
			 * @param[in] forkMasterController Whether to fork the controller.
			 * @return Reference to object.
			 */
			BVS& start(bool forkMasterController = true);

			/** Tells the system to keep running until paused or quit.
			 * This will signal the system's controller to keep issuing new rounds/
			 * steps until it is paused.
			 * @return Reference to object.
			 * @see BVS::start
			 */
			BVS& run();

			/** Tells the system to advance by one round/step.
			 * This will signal the system's controller to issue a new round/
			 * step after which it will pause and wait for further signals.
			 * @return Reference to object.
			 */
			BVS& step();

			/** Tells the system to pause.
			 * This will signal the system's controller to issue a pause signal to
			 * all modules after which it will wait for further signals.
			 * @return Reference to object.
			 */
			BVS& pause();

			/** HotSwap a module.
			 * This will reload/hotswap an already existing module.
			 *
			 * \note Only works if there is only ONE module instance of a
			 * library present.
			 *
			 * \warning This is inherently not safe and should ONLY be used in a
			 * development scenario. HotSwapping is likely to crash/segfault if
			 * not used properly (for more info see BVS::Loader).
			 *
			 * \warning The function will pause the system and tell the loader
			 * to hotswap the library. Afterwards the system will be put into
			 * the same state as before.
			 * @param[in] id Module ID to hotswap (if it does not exist, it fails silently).
			 */
			BVS& hotSwap(const std::string& id);

			/** Tells the system to quit.
			 * This will signal the system's controller to issue a quit signal to
			 * all modules after which it will start to shutdown the entire system
			 * by unloading all modules properly.
			 * @return Reference to object.
			 */
			BVS& quit();

			Config config; /**< BVS' config system. */
			std::function<void()> shutdownHandler; /**< Function to call when shutting system down. */

		private:
			Info info; //**< BVS' information object. */
#ifdef BVS_LOG_SYSTEM
			std::shared_ptr<LogSystem> logSystem; /**< Internal log system backend. */
			Logger logger; /**< BVS' logging instance. */
#endif
			Loader* loader; /**< BVS' module loader. */
			Control* control; /**< BVS' module controller. */
			std::stack<std::string> moduleStack; /** Stack of modules names. */

			BVS(const BVS&) = delete; /**< -Weffc++ */
			BVS& operator=(const BVS&) = delete; /**< -Weffc++ */
	};
} // namespace BVS



#endif //BVS_H

