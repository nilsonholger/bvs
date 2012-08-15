#ifndef BVS_H
#define BVS_H

#include<iomanip>
#include<iostream>
#include<string>

#include "bvs/bvsinfo.h"
#include "bvs/config.h"
#include "bvs/connector.h"
#include "bvs/logger.h"
#include "bvs/traits.h"



/** @mainpage The BVS framework.
 * Here be dragons ;-)
 */



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	// Forward declarations
	class Control;
	class Loader;



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
	 * @li \c modules lists modules to load and their options.
	 *
	 * To set the verbosity level of different/any logger instance (belongs to section [Logger]):
	 * @code
	 * BVS = 0
	 * Daemon = 0
	 * Master = 0
	 * YourLogger = ...
	 * @endcode
	 */
	class BVS
	{
		public:
			/** Create BVS System.
			 * @param[in] argc Main's argc.
			 * @param[in] argv Main's argv, used to pass config options to BVS, see BVSConfig.
			 */
			BVS(int argc, char** argv);

			/** Destructor.
			 */
			~BVS();

			/** Load modules selected by config variable [BVS]modules.
			 * @return Reference to object.
			 */
			BVS& loadModules();

			/** Load selected module given by name.
			 * @param[in] id The name of the module.
			 * @param[in] asThread Select, if the module should run in it's own thread.
			 * @return Reference to object.
			 */
			BVS& loadModule(const std::string& id, bool asThread = false);

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

			/** Tells the system to quit.
			 * This will signal the system's controller to issue a quit signal to
			 * all modules after which it will start to shutdown the entire system
			 * by unloading all modules properly.
			 * @return Reference to object.
			 */
			BVS& quit();

			Config config; /**< BVS' config system. */

		private:
			Info info; //**< BVS' information object. */
			std::shared_ptr<LogSystem> logSystem; /**< Internal log system backend. */
			Logger logger; /**< BVS' logging instance. */
			Control* control; /**< BVS' module controller. */
			Loader* loader; /**< BVS' module loader. */

			BVS(const BVS&) = delete; /**< -Weffc++ */
			BVS& operator=(const BVS&) = delete; /**< -Weffc++ */
	};
} // namespace BVS



#endif //BVS_H

