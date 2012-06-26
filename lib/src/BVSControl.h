#ifndef BVSCONTROL_H
#define BVSCONTROL_H

#include<atomic>
#include<condition_variable>
#include<mutex>
#include<thread>

#include "BVSConfig.h"
#include "BVSLoader.h"
#include "BVSLogger.h"
#include "BVSModuleData.h"



/** Possible commands to send to masterController, see sendCommand(). */
enum class BVSSystemFlag { QUIT = 0, PAUSE = 1, RUN = 2, STEP = 3, STEP_BACK = 4 };



/** The system control: starts, stops and controls modules in general. */
class BVSControl
{
	public:
		/** Constructor for control.
		 */
		BVSControl();

		/** The master control function.
		 * This is the master control function, it forks if desired and can be controlled
		 * by using sendCommand(...).
		 * @param[in] forkMasterController To fork or not to fork.
		 * @return Reference to object.
		 */
		BVSControl& masterController(const bool forkMasterController = true);

		/** Send commands to master control.
		 * This function sends commands to the master control. If the master controller
		 * has forked, it will return immediately after dropping its flag.
		 * Beware: if the master has not forked and a RUN flag is send, this function
		 * will loop *forever* and never return.
		 * Possible flags (BVSSystemFlag::...):
		 * QUIT      - sends quitting signal to all modules.
		 * PAUSE     - pauses system.
		 * RUN       - runs system until another command is send.
		 * STEP      - advances the system by one step/round.
		 * STEP_BACK (not yet implemented)
		 * @param controlFlag The desired command to be submitted to server.
		 * @return Reference to object.
		 */
		BVSControl& sendCommand(const BVSSystemFlag controlFlag = BVSSystemFlag::PAUSE);

	private:
		/** Controls given module.
		 * @param[in] data Module meta data.
		 * @return Reference to object.
		 */
		BVSControl& moduleController(BVSModuleData& data);

		/** Controls a module started as a thread.
		 * @param[in] data Module meta data.
		 * @return Reference to object.
		 */
		BVSControl& threadController(std::shared_ptr<BVSModuleData> data);

		/** The number of actively running threads. */
		static std::atomic<int> runningThreads;

		/** The number of modules running in threads. */
		static int threadedModules;

		/** The active system flag used by master. */
		BVSSystemFlag flag;

		BVSLogger logger; /**< Logger metadata. */

		std::mutex masterMutex; /**< Mutex for masterController. */
		std::unique_lock<std::mutex> masterLock; /**< Lock for masterController. */
		std::condition_variable masterCond; /**< Condition variable for masterController. */

		std::mutex threadMutex; /**< Mutex for threadController. */
		std::condition_variable threadCond; /**< Condition variable for threadController. */

		std::thread controlThread; /**< Thread (if active) of masterController. */

		long long round; /**< System round counter. */

		BVSControl(const BVSControl&) = delete; /**< -Weffc++ */
		BVSControl& operator=(const BVSControl&) = delete; /**< -Weffc++ */

		friend class BVSLoader;
};



#endif //BVSCONTROL_H

