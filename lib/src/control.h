#ifndef BVS_CONTROL_H
#define BVS_CONTROL_H

#include<atomic>
#include<chrono>
#include<condition_variable>
#include<mutex>
#include<thread>

#include "bvs/bvsinfo.h"
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
			Control(Info& info);

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

			/** Create new thread for a module.
			 * This creates a new thread for given module. It will connect the
			 * created thread to the thread handle inside the supplied
			 * metadata.
			 * @param[in] data Shared pointer to module metadata.
			 * @return Reference to object.
			 */
			Control& createModuleThread(std::shared_ptr<ModuleData> data);

			/** Notify all threads.
			 * Note that this does NOT send any command, it merely notifies
			 * all threads. This is useful for example if you want to send a
			 * signal to one thread only, e.g. sending a QUIT.
			 * @return Reference to object.
			 */
			Control& notifyThreads();

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

			/** Info reference. */
			Info& info;

			/** The active system flag used by master. */
			SystemFlag flag;

			Logger logger; /**< Logger metadata. */

			std::mutex mutex; /**< Mutex for condition variable. */
			std::unique_lock<std::mutex> masterLock; /**< Lock for masterController. */
			std::condition_variable monitor; /**< Condition variable for masterController. */

			std::thread controlThread; /**< Thread (if active) of masterController. */

			unsigned long long round; /**< System round counter. */

			/** Timer keeping the start time for a round. */
			std::chrono::time_point<std::chrono::high_resolution_clock> timer;

			/** Timer for master controlled modules. */
			std::chrono::time_point<std::chrono::high_resolution_clock> timer2;

			Control(const Control&) = delete; /**< -Weffc++ */
			Control& operator=(const Control&) = delete; /**< -Weffc++ */
	};
} // namespace BVS



#endif //BVS_CONTROL_H

