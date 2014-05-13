
#ifndef EMGPROVIDER_H
#define EMGPROVIDER_H

#include <thread>
#include "BlockingQueue.h"
#include "Interval.h"

/**
  * Defines the current status of the EMGProvider. NEW means the
  * EMGProvider was created, but never started. RUNNING means the
  * EMGProvider is currently reading values. WAITING means the 
  * EMGProvider is currently stoped, but it is possible to start
  * it again. FINISHED means that the EMGProvider has read all
  * values or was shutdown. It is not possible to start it again.
  */
enum Status {
	NEW, RUNNING, WAITING, FINISHED
};

/**
  * Defines the signals, that can be send to the EMGProvider.
  * START starts the EMGProvider and STOP stops it. It is
  * possible to send this two signals alternating. SHUTDOWN
  * stops the EMGProvider. It is not possible to start it again.
  */
enum Signal {
	START, STOP, SHUTDOWN
};

/**
  * The EMGProvider defines an abstract class that is responsible
  * for reading EMG signals. There are two different implementations
  * of the EMGProvider, the EMGFileProver and <TODO: ADD_NAME>. Both
  * classes start a new thread to read the EMG signals parallel to
  * the classification process. It is possible to communicate with
  * the reading thread via the send(const Signal&) member function.
  */
class EMGProvider {
protected:
	std::thread thread;

	Status status = NEW;
	BlockingQueue<Interval*> intervals;
	Interval *lastInterval = NULL;
	void addInterval(Interval* const interval);

public:
	~EMGProvider();

	/**
	  * Returns an Interval that was created by the EMGProvider. When
	  * several intervals are available the oldest is returned. When
	  * no Interval is available the calling thread will be blocked
	  * until an Interval is available.
	  * It is necessary to delete the Interval.
	  */
	Interval* getInterval();

	/**
	  * Removes all cached Intervals from the buffer and deletes
	  * them.
	  */
	void clearBuffer();

	/**
	  * Sends a signal to the reading thread. See Signal for more
	  * information.
	  */
	virtual void send(const Signal& signal) = NULL;
};

#endif