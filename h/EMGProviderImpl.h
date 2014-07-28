#ifndef EMGPROVIDERIMPL_H
#define EMGPROVIDERIMPL_H

#include <thread>
#include "AppConfig.h"
#include "BlockingQueue.h"
#include "EMGProvider.h"

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
	class EMGProviderImpl : public EMGProvider {
	private:
		//Logs a warning when bufferWarnSize much Intervals are cached.
		int bufferWarnSize = AppConfig::getInstance()->getEMGProviderBufferWarning();

	protected:
		std::thread thread;
		//The size of cached Intervals is limited. The least recent Interval will be deleted.
		BlockingQueue<Interval*> intervals{ 10000, AppConfig::getInstance()->getBlockingQueueMaxWaitTime() };
		Interval *lastInterval = NULL;
		Status status = Status::NEW;

		void addInterval(Interval* const interval);

	public:

		/**
		  * Deletes all cached Intervals.
		  */
		virtual ~EMGProviderImpl();

		virtual Interval* getInterval();
		
		virtual void clearBuffer();

		virtual int getBufferSize() const;

		virtual int getSampleNr() const;

		virtual void send(const Signal& signal) = NULL;
	};
}

#endif