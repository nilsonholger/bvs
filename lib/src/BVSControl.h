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



// TODO comment
enum class BVSSystemFlag { QUIT = 0, PAUSE = 1, RUN = 2, STEP = 3, STEP_BACK = 4 };



/** The system control: starts, stops and controls modules in general. */
class BVSControl
{
	public:
		/** Constructor for control.
		 */
		BVSControl();

		// TODO comment
		BVSControl& masterController(const bool forkMasterController = true);

		// TODO comment
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

		// TODO comment
		BVSSystemFlag flag;

		BVSLogger logger; /**< Logger metadata. */

		std::mutex masterMutex;
		std::unique_lock<std::mutex> masterLock;
		std::condition_variable masterCond;

		std::mutex threadMutex;
		std::condition_variable threadCond;

		std::thread controlThread;

		long long round;

		BVSControl(const BVSControl&) = delete; /**< -Weffc++ */
		BVSControl& operator=(const BVSControl&) = delete; /**< -Weffc++ */

		// TODO fix
		friend BVSLoader;
};



#endif //BVSCONTROL_H

