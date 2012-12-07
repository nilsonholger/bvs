#include "barrier.h"



BVS::Barrier::Barrier()
	: mutex{},
	cv{},
	parties{0},
	queued{0}
{ }



std::unique_lock<std::mutex> BVS::Barrier::attachParty()
{
	parties.fetch_add(1);

	return {mutex, std::defer_lock};
}



BVS::Barrier& BVS::Barrier::detachParty()
{
	parties.fetch_sub(1);

	return *this;
}



BVS::Barrier& BVS::Barrier::enqueue(std::unique_lock<std::mutex>& lock)
{
	enqueue(lock, [&](){ return queued.load()==parties.load(); });

	return *this;
}



BVS::Barrier& BVS::Barrier::enqueue(std::unique_lock<std::mutex>& lock, std::function<bool()> predicate)
{
	lock.lock();
	queued.fetch_add(1);
	if (queued.load()==parties.load()) cv.notify_all();
	cv.wait(lock, predicate);
	queued.fetch_sub(1);
	lock.unlock();

	return *this;
}



BVS::Barrier& BVS::Barrier::notify()
{
	cv.notify_all();

	return *this;
}
