#ifndef BVS_BARRIER_H
#define BVS_BARRIER_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** Barrier to synchronize/rendezvous a various number of threads/parties.
	 * The last party to arrive at the barrier will wake up all other parties.
	 * @code
	 * Barrier barrier;
	 * std::unique_lock<std::mutex> lock{barrier.attachParty()};
	 *
	 * barrier.enqueue(lock); // lock must be in an unlocked state
	 * @endcode
	 *
	 * For advanced usage, a function predicate can be given while waiting on
	 * the barrier. This allows for a spurious wakeup of a party after a
	 * notify().
	 * @code
	 * // one party
	 * barrier.enqueue(lock, [&](){ return spuriousWakeUp==true; });
	 *
	 * // another party or a controller
	 * barrier.notify();
	 * @endcode
	 */
	class Barrier
	{
		public:
			/** Barrier constructor. */
			Barrier();

			/** Attach a party to the barrier pool.
			 * Increases internal member count.
			 * @return A unique lock in unlocked state.
			 */
			std::unique_lock<std::mutex> attachParty();

			/** Detach a party from the barrier pool.
			 * Decreases internal member count.
			 * @return Reference to object.
			 */
			Barrier& detachParty();

			/** Wait on barrier until all registered parties arrive.
			 * If the thread calling enqueue(...) is the last to arrive, it
			 * will wake up all others, if not, it will resume once all
			 * others have arrived.
			 * @param[in] lock The associated lock in an unlocked state.
			 * @return Reference to object.
			 */
			Barrier& enqueue(std::unique_lock<std::mutex>& lock);

			/** Wait on barrier.
			 * Wait on barrier until predicate is true. The predicate will be
			 * checked if either all parties arrived at the barrier or there
			 * has been a 'notify()'.
			 * @param[in] lock The associated lock in an unlocked state.
			 * @param[in] predicate A function predicate to use as a wake up check, see Barrier's example.
			 * @return Reference to object.
			 */
			Barrier& enqueue(std::unique_lock<std::mutex>& lock, std::function<bool()> predicate);

			/** Notify all pool parties.
			 * This is useful when parties are using self-determined predicates.
			 * @return Reference to object.
			 */
			Barrier& notify();

		private:
			std::mutex mutex; /**< Mutex for parties' locks. */
			std::condition_variable cv; /**< Condition variable for parties. */
			std::atomic<int> parties; /**< Number of parties. */
			std::atomic<int> queued; /**< Number of currently waiting parties on condition variable. */
	};
} // namespace BVS

#endif //BVS_BARRIER_H

