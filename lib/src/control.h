#ifndef BVS_CONTROL_H
#define BVS_CONTROL_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "bvs/bvsinfo.h"
#include "bvs/logger.h"
#include "controldata.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** Forward declaration. */
	class Loader;



	/** Possible commands to send to masterController, see sendCommand(). */
	enum class SystemFlag { QUIT = 0, PAUSE = 1, RUN = 2, STEP = 3, STEP_BACK = 4 };



	/** The system control: starts, stops and controls modules in general. */
	class Control
	{
		public:
			/** Constructor for control.
			*/
			Control(Info& info);

			/** Registers a module.
			 * @param[in] id Name of module.
			 * @param[in] module Pointer to module.
			 * @param[in] TODO
			 */
			static void registerModule(const std::string& id, Module* module, bool hotSwap = false);

			/** The master control function.
			 * This is the master control function, it forks if desired and can
			 * be controlled by using sendCommand(...).
			 * @param[in] forkMasterController To fork or not to fork.
			 * @return Reference to object.
			 */
			Control& masterController(const bool forkMasterController = true);

			/** Send commands to master control.
			 * This function sends commands to the master control. If the
			 * master controller has forked, it will return immediately after
			 * dropping its flag.
			 *
			 * Beware: if the master has not forked and a RUN flag is send,
			 * this function will loop *forever* and never return.
			 *
			 * Possible flags (SystemFlag::...):
			 * @li \c QUIT      - sends quitting signal to all modules.
			 * @li \c PAUSE     - pauses system.
			 * @li \c RUN       - runs system until another command is send.
			 * @li \c STEP      - advances the system by one step/round.
			 * @li \c STEP_BACK (not yet implemented)
			 * @param controlFlag The desired command to be submitted to server.
			 * @return Reference to object.
			 */
			Control& sendCommand(const SystemFlag controlFlag = SystemFlag::PAUSE);

			/** TODO comment */
			SystemFlag queryActiveFlag();

			/** Start a module.
			 * This will start a module. It will act according to its metadata,
			 * so it will be controlled either by the master, separate as a
			 * thread or be part of a module pool.
			 * @param[in] id Module id to start.
			 * @return Reference to object.
			 */
			Control& startModule(std::string id);

			/** Notify all threads.
			 * Note that this does NOT send any command, it merely notifies
			 * all threads. This is useful for example if you want to send a
			 * signal to one thread only, e.g. sending a QUIT.
			 * @return Reference to object.
			 */
			Control& notifyThreads();

			/** Purge module from internal data.
			 * @param[in] moduleID Name of module to remove from internal data.
			 * @return Reference to object.
			 */
			Control& purgeData(std::string moduleID);

			/** Map of registered modules and their metadata. */
			static ModuleDataMap modules;

		private:
			/** Controls given module.
			 * @param[in] data Module meta data.
			 * @return Reference to object.
			 */
			Control& moduleController(ModuleData& data);

			/** Controls a module started as a thread.
			 * @param[in] data Module meta data.
			 * @return Reference to object.
			 */
			Control& threadController(std::shared_ptr<ModuleData> data);

			/** Control a module pool.
			 * @param[in] data Pool meta data.
			 * @return Reference to object.
			 */
			Control& poolController(std::shared_ptr<PoolData> data);

			Info& info; /**< Info reference. */

			Logger logger; /**< Logger metadata. */

			std::atomic<int> runningThreads; /**< The number of actively running threads. */

			ModuleDataVector masterModules; /**< Vector of modules executed by master. */

			PoolMap pools; /**< Map of pools. */

			SystemFlag flag; /**< The active system flag used by master. */

			std::mutex mutex; /**< Mutex for condition variable. */
			std::unique_lock<std::mutex> masterLock; /**< Lock for masterController. */
			std::condition_variable monitor; /**< Condition variable for masterController. */
			std::thread controlThread; /**< Thread (if active) of masterController. */

			unsigned long long round; /**< System round counter. */

			static ModuleVector* hotSwapGraveYard; /** TODO */

			Control(const Control&) = delete; /**< -Weffc++ */
			Control& operator=(const Control&) = delete; /**< -Weffc++ */
	};
} // namespace BVS



#endif //BVS_CONTROL_H

