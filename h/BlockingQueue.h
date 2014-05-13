
#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <condition_variable>
#include <limits>
#include <mutex>
#include <queue>
#include "Interval.h"

/**
  * This class can be used to pass messages/objects between threads. When the queue
  * is empty the calling thread will be blocked until an item is available. It is
  * possible to define a maximum queue size, so that the first element will be removed
  * when the maximum size is reached.
  */
template<typename T>
class BlockingQueue {
private:
	std::queue<T> queue;
	std::mutex mutex;
	std::condition_variable cond;
	int maxSize;
public:

	/**
	  * Creates a new BlockingQueue with the given maximal size. The default value
	  * is the maximal value for an Integer.
	  */
	BlockingQueue(int maxSize = std::numeric_limits<int>::max()) {
		BlockingQueue::maxSize = maxSize;
	}

	/**
	  * Pops the first element from the queue. If the queue is empty, the calling
	  * thread will be blocked until at least one element is again stored in the
	  * queue.
	  * TODO: it should be possible to define a maximum waiting time
	  */
	T pop() {
		std::unique_lock<std::mutex> mlock(mutex);
		while (queue.empty())
			cond.wait(mlock);
		auto item = queue.front();
		queue.pop();
		return item;
	}

	/**
	  * Pushes the given element into the queue. When a maximum queue size was
	  * defined and the maximum size is reached, the first element from the
	  * queue will be removed. When a thread is waiting for this queue
	  * the thread will be notified, when the given item was stored in the queue.
	  */
	void push(const T& item) {
		std::unique_lock<std::mutex> mlock(mutex);

		//removes the first item from the queue, so that the maximum size is still correct
		if (queue.size() == maxSize)
			queue.pop();
		queue.push(item);
		mlock.unlock();
		cond.notify_one();
	}

	/**
	  * Test whether this queue is empty. This function never blocks the
	  * calling thread.
	  */
	bool empty() const {
		return queue.empty();
	}
};

#endif