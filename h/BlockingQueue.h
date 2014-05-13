
#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include "Interval.h"

template<typename T>
class BlockingQueue {
private:
	std::queue<T> queue;
	std::mutex mutex;
	std::condition_variable cond;
public:
	// pop an element and return a copy. Block if queue empty.
	T pop() {
		std::unique_lock<std::mutex> mlock(mutex);
		while (queue.empty())
			cond.wait(mlock);
		auto item = queue.front();
		queue.pop();
		return item;
	}

	//  push an element to the back of the queue.
	void push(const T& item) {
		std::unique_lock<std::mutex> mlock(mutex);
		queue.push(item);
		mlock.unlock();
		cond.notify_one();
	}

	bool empty() const {
		return queue.empty();
	}
};

#endif