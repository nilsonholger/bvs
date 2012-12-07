#ifndef BVS_BARRIER_H
#define BVS_BARRIER_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>



//TODO comment, especially correct usage
namespace BVS
{
	class Barrier
	{
		public:
			Barrier();
			std::unique_lock<std::mutex> attachParty();
			Barrier& detachParty();
			Barrier& enqueue(std::unique_lock<std::mutex>& lock);
			Barrier& enqueue(std::unique_lock<std::mutex>& lock, std::function<bool()> predicate);
			Barrier& notify();
		private:
			std::mutex mutex;
			std::condition_variable cv;
			std::atomic<int> parties;
			std::atomic<int> queued;
	};
}

#endif //BVS_BARRIER_H

