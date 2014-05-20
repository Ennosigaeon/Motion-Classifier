#ifndef EMGPROVIDER_H
#define EMGPROVIDER_H

#include <thread>
#include "AppConfig.h"
#include "BlockingQueue.h"
#include "Communication.h"
#include "Interval.h"

/**
  * The EMGProvider defines an abstract class that is responsible
  * for reading EMG signals. There are two different implementations
  * of the EMGProvider, the EMGFileProver and <TODO: ADD_NAME>. Both
  * classes start a new thread to read the EMG signals parallel to
  * the classification process. It is possible to communicate with
  * the reading thread via the send(const Signal&) member function.
  */
class EMGProvider {
private:
	//Logs a warning when too much Intervals are cached.
	static int bufferWarnSize;

protected:
	std::thread thread;

	Status status = NEW;
	BlockingQueue<Interval*> intervals{ 1000, AppConfig::getInstance()->getBlockingQueueMaxWaitTime() };
	Interval *lastInterval = NULL;
	void addInterval(Interval* const interval);

public:
	~EMGProvider();

	/**
	  * Returns an Interval that was created by the EMGProvider. When
	  * several intervals are available the oldest is returned. When
	  * no Interval is available the calling thread will be blocked
	  * until an Interval is available.
	  * When a maximum wait time for the BlockingQueue is defined, it
	  * is possible that this function returns NULL.
	  * It is necessary to delete the Interval.
	  */
	Interval* getInterval();

	/**
	  * Removes all cached Intervals from the buffer and deletes
	  * them.
	  */
	void clearBuffer();

	/**
	  * Sends a signal to the reading thread. See Signal (in
	  * Communication.h) for more information.
	  */
	virtual void send(const Signal& signal) = NULL;
};

#endif