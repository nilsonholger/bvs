#ifndef BVS_CONTROL_H
#define BVS_CONTROL_H

#include <atomic>
#include <thread>

#include "bvs/bvs.h"
#include "bvs/info.h"
#include "bvs/logger.h"
#include "barrier.h"
#include "controldata.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** Possible commands to send to masterController, see sendCommand(). */
	enum class SystemFlag { QUIT = 0, PAUSE = 1, RUN = 2, STEP = 3, STEP_BACK = 4 };



	/** The system control: starts, stops and controls modules in general. */
	class Control
	{
		public:
			/** Constructor for control.
			 * @param[in] modules Reference to module meta data map.
			 * @param[in] bvs Referecence to bvs.
			 * @param[in] info Reference to info struct.
			*/
			Control(ModuleDataMap& modules, BVS& bvs, Info& info);

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

			/** Query the system for the actively used flag.
			 * @return SystemFlag in use by system.
			 */
			SystemFlag queryActiveFlag();

			/** Start a module.
			 * This will start a module. It will act according to its metadata,
			 * so it will be controlled either by the master, separate as a
			 * thread or be part of a module pool.
			 * @param[in] id Module id to start.
			 * @return Reference to object.
			 */
			Control& startModule(std::string id);

			/** Signal a module to quit.
			 * This will send a 'QUIT' signal to the specified module.
			 * @param[in] id Module id to signal quit to.
			 * @return Reference to object.
			 */
			Control& quitModule(std::string id);

			/** Purge module from internal data.
			 * @param[in] id Name of module to remove from internal data.
			 * @return Reference to object.
			 */
			Control& purgeData(const std::string& id);

			/** Wait until given module is inactive.
			 * Wait until the given module is inactive. If this never happens,
			 * this call will loop forever.
			 * @param[in] id Module id to wait for.
			 * @return Reference to object.
			 */
			Control& waitUntilInactive(const std::string& id);

			/** Check if module is active.
			 * Check if the given module is being actively run by the master, a
			 * thread or as part of a pool as of RIGHT AT THAT MOMENT!
			 * @param[in] id Module id to check status for.
			 * @return True if active, false if not.
			 */
			bool isActive(const std::string& id);

			ModuleDataMap& modules; /**< Reference to module meta data map. */

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

			/** Check module status and act upon it if necessary. */
			Control& checkModuleStatus(std::shared_ptr<ModuleData> data);

			BVS& bvs; /**< BVS reference. */
			Info& info; /**< Info reference. */
			Logger logger; /**< Logger metadata. */
			std::atomic<int> activePools; /**< The number of active pools. */
			ModuleDataVector masterPoolModules; /**< Vector of modules executed by master (default pool). */
			PoolMap pools; /**< Map of pools. */
			SystemFlag flag; /**< The active system flag used by master. */

			Barrier barrier; /**< Pool synchronization barrier. */
			std::unique_lock<std::mutex> masterLock; /**< Lock for masterController. */
			std::thread controlThread; /**< Thread (if active) of masterController. */

			unsigned long long round; /**< System round counter. */
			bool shutdownRequested; /**< True if shutdown was requested. */
			unsigned long long shutdownRound; /**< System shutdown round. */

			Control(const Control&) = delete; /**< -Weffc++ */
			Control& operator=(const Control&) = delete; /**< -Weffc++ */
	};
} // namespace BVS



#endif //BVS_CONTROL_H

