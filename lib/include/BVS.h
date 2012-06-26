#ifndef BVS_H
#define BVS_H

#include<iomanip>
#include<iostream>
#include<string>

#include "BVSConfig.h"
#include "BVSLogger.h"
#include "BVSTraits.h"



// Forward declarations
class BVSConnector;
class BVSControl;
class BVSLoader;



/** The BVS framework base.
 * This is the BVS base, use this to interact with the framework.
 * TODO COMMENTS for BVS settings... and maybe usage
 * TODO finish callthrougs for all BVS sub parts
 * TODO sort functions by usage/group
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
		 * @param[in] identifier The name of the module.
		 * @param[in] asThread Select, if the module should run in it's own thread.
		 * @return Reference to object.
		 */
		BVS& loadModule(const std::string& identifier, bool asThread = false);

		/** Unload module given by name.
		 * @param[in] identifier The name of the module.
		 * @return Reference to object.
		 */
		BVS& unloadModule(const std::string& identifier);

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

		// TODO build data exchange between modules
		BVS& connectModules();

		// TODO comment IMPORTANT, maybe include forkMasterController, explain difference in usage
		BVS& start();
		BVS& run();
		BVS& step();
		BVS& pause();
		//void stop();
		BVS& quit();

		BVSConfig config; /**< BVS' config system. */

	private:
		std::shared_ptr<BVSLogSystem> logSystem; /**< Internal log system backend. */
		BVSLogger logger; /**< BVS' logging instance. */
		BVSControl* control; /**< BVS' module loader. */
		BVSLoader* loader;

		//BVSConnectorList& connectors;

		BVS(const BVS&) = delete; /**< -Weffc++ */
		BVS& operator=(const BVS&) = delete; /**< -Weffc++ */
};



#endif //BVS_H

