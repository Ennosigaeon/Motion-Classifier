#ifndef EMGPROVIDER_H
#define EMGPROVIDER_H

#include "Communication.h"
#include "Interval.h"

namespace motion_classifier {

	class EMGProvider {
	protected:
		int sampleNr = 0;

	public:

		/**
		  * Returns the number of the last read Sample.
		  */
		virtual int getSampleNr() const = NULL;
		
		/**
		  * Returns an recorded Interval. When several intervals are
		  * available the oldest is returned. When no Interval is
		  * available the calling thread will be blocked until an
		  * Interval is available. When a maximum wait time for the
		  * BlockingQueue is defined and the calling thread has
		  * exceeded this waiting time NULL will be returned. It is
		  * necessary to delete the Interval, when it is not needed
		  * anymore.
		  */
		virtual Interval* getInterval() = NULL;

		/**
		  * Removes all cached Intervals from the buffer and deletes
		  * them.
		  */
		virtual void clearBuffer() = NULL;

		/**
		  * Returns the number of stored Intervals.
		  */
		virtual int getBufferSize() const = NULL;

		/**
		  * Sends a signal to the reading process. After the
		  * creation the worker is not reading any EMG signals.
		  * To start the reading procedure send Signal::START.
		  * The worker will create new Intervals until it is not
		  * possible anymore (reached end of file, socket is
		  * closed, ...). To stop the recording send Signal::STOP.
		  * The reading process is stoped, no more EMG signals
		  * are recorded. You are able to start the recording
		  * again by sending Signal::START. To permanently stop
		  * the reading send Signal::SHUTDOWN. This will stop the
		  * worker and (maybe) destroy it. It is not possible to
		  * start the worker again. Signal::SHUTDOWN is automatically
		  * send when the EMGProvider is destroyed.
		  */
		virtual void send(const Signal& signal) = NULL;
	};
}

#endif