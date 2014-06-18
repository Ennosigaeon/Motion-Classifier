#ifndef EMGPROVIDER_H
#define EMGPROVIDER_H

#include <thread>
#include "AppConfig.h"
#include "BlockingQueue.h"
#include "Communication.h"
#include "Interval.h"

namespace motion_classifier {
	/**
	  * The EMGProvider defines an abstract class that is responsible
	  * for reading EMG signals. There are two given implementations
	  * of the EMGProvider, the EMGFileProver and EMGOTProvider. Both
	  * classes read EMG signals parallel to the classification process.
	  * It is possible to communicate with the reading worker via the
	  * send(const Signal&) member function. In the Readme file is a
	  * detailed description how to use these two implemenations of the
	  * EMGProvider. You can add own implementation of EMGProvider
	  * when you have an different source for the EMG signals.
	  */
	class EMGProvider {
	private:
		//Logs a warning when bufferWarnSize much Intervals are cached.
		int bufferWarnSize = AppConfig::getInstance()->getEMGProviderBufferWarning();

	protected:
		std::thread thread;
		Status status = Status::NEW;
		//The size of cached Intervals is limited. The least recent Interval will be deleted.
		BlockingQueue<Interval*> intervals{ 10000, AppConfig::getInstance()->getBlockingQueueMaxWaitTime() };
		Interval *lastInterval = NULL;
		long sampleNr;

		void addInterval(Interval* const interval);

	public:

		/**
		  * Deletes all cached Intervals.
		  */
		~EMGProvider();

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
		Interval* getInterval();

		/**
		  * Removes all cached Intervals from the buffer and deletes
		  * them.
		  */
		void clearBuffer();

		/**
		  * Returns the number of stored Intervals.
		  */
		int getBufferSize() const;

		/**
		  * Returns the number of the last read Sample.
		  */
		int getSampleNr() const;

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