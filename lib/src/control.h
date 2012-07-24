#ifndef BVS_CONTROL_H
#define BVS_CONTROL_H

#include<atomic>
#include<condition_variable>
#include<mutex>
#include<thread>

#include "bvs/config.h"
#include "bvs/logger.h"
#include "loader.h"
#include "moduledata.h"



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
			*/
			Control();

			/** The master control function.
			 * This is the master control function, it forks if desired and can be controlled
			 * by using sendCommand(...).
			 * @param[in] forkMasterController To fork or not to fork.
			 * @return Reference to object.
			 */
			Control& masterController(const bool forkMasterController = true);

			/** Send commands to master control.
			 * This function sends commands to the master control. If the master controller
			 * has forked, it will return immediately after dropping its flag.
			 * Beware: if the master has not forked and a RUN flag is send, this function
			 * will loop *forever* and never return.
			 * Possible flags (SystemFlag::...):
			 * QUIT      - sends quitting signal to all modules.
			 * PAUSE     - pauses system.
			 * RUN       - runs system until another command is send.
			 * STEP      - advances the system by one step/round.
			 * STEP_BACK (not yet implemented)
			 * @param controlFlag The desired command to be submitted to server.
			 * @return Reference to object.
			 */
			Control& sendCommand(const SystemFlag controlFlag = SystemFlag::PAUSE);

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

			/** The number of actively running threads. */
			static std::atomic<int> runningThreads;

			/** The number of modules running in threads. */
			static int threadedModules;

			/** The active system flag used by master. */
			SystemFlag flag;

			Logger logger; /**< Logger metadata. */

			std::mutex masterMutex; /**< Mutex for masterController. */
			std::unique_lock<std::mutex> masterLock; /**< Lock for masterController. */
			std::condition_variable masterCond; /**< Condition variable for masterController. */

			std::mutex threadMutex; /**< Mutex for threadController. */
			std::condition_variable threadCond; /**< Condition variable for threadController. */

			std::thread controlThread; /**< Thread (if active) of masterController. */

			long long round; /**< System round counter. */

			Control(const Control&) = delete; /**< -Weffc++ */
			Control& operator=(const Control&) = delete; /**< -Weffc++ */

			friend class Loader;
	};
} // namespace BVS



#endif //BVS_CONTROL_H

