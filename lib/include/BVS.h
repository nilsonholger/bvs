#ifndef BVS_H
#define BVS_H

#include<iomanip>
#include<iostream>
#include<string>

#include "BVSConfig.h"
#include "BVSConnector.h"
#include "BVSLogger.h"
#include "BVSTraits.h"



// Forward declarations
class BVSControl;
class BVSLoader;



/** The BVS framework base.
 * This is the BVS base, use this to interact with the framework.
 */
class BVS
{
	public:
		/** Create BVS System.
		 * @param[in] argc Main's argc.
		 * @param[in] argv Main's argv, used to pass config options to BVS, see BVSConfig.
		 */
		BVS(int argc, char** argv);

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
		BVS& connectModules();

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

		BVSConfig config; /**< BVS' config system. */

	private:
		std::shared_ptr<BVSLogSystem> logSystem; /**< Internal log system backend. */
		BVSLogger logger; /**< BVS' logging instance. */
		BVSControl* control; /**< BVS' module controller. */
		BVSLoader* loader; /**< BVS' module loader. */

		BVS(const BVS&) = delete; /**< -Weffc++ */
		BVS& operator=(const BVS&) = delete; /**< -Weffc++ */
};



#endif //BVS_H

