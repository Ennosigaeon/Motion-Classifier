#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <chrono>
#include <condition_variable>
#include <limits>
#include <mutex>
#include <queue>

namespace motion_classifier {
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
		int maxWaitTime;

	public:

		/**
		  * Creates a new BlockingQueue with the given maximal size. The default value
		  * is the maximal value for an Integer.
		  * It is also possible to specify the maximum wait time in milliseconds for
		  * the pop() operation. When the timeout is reached NULL will be returned.
		  * The default value is -1. When this parameter is set to an other value T
		  * has to be a pointer.
		  */
		BlockingQueue(int maxSize = std::numeric_limits<int>::max(), int maxWaitTime = -1) {
			BlockingQueue::maxSize = maxSize;
			BlockingQueue::maxWaitTime = maxWaitTime;
		}

		/**
		  * Pops the first element from the queue. If the queue is empty, the calling
		  * thread will be blocked until at least one element is again stored in the
		  * queue.
		  * When a maximum waiting time was specified in the constructor the calling
		  * thread will be notified when the timeout is reached. Then pop() returns
		  * NULL.
		  */
		T pop() {
			std::unique_lock<std::mutex> mlock(mutex);
			if (maxWaitTime != -1) {
				cond.wait_for(mlock, std::chrono::milliseconds(maxWaitTime));
				if (queue.empty())
					return NULL;
			}
			else
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

		/**
		  * Returns the current number of stored elements.
		  */
		int size() const {
			return queue.size();
		}

		void resize(unsigned int size) {
			maxSize = size;
			while (queue.size() > maxSize)
				pop();
		}
	};
}

#endif