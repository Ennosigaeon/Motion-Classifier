
#ifndef EMGPROVIDER_H
#define EMGPROVIDER_H

#include <thread>
#include "BlockingQueue.h"
#include "Interval.h"

enum Status {
	RUNNING, WAITING, FINISHED
};

enum Signal {
	START, STOP, SHUTDOWN
};

class EMGProvider {
protected:
	std::thread thread;

	Status status = WAITING;
	BlockingQueue<Interval*> intervals;
	Interval *lastInterval = NULL;
	void addInterval(Interval* const interval);

public:
	~EMGProvider();

	Interval* getInterval();
	void clearBuffer();
	virtual void send(const Signal& signal) = NULL;
};

#endif