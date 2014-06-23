#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "AppConfig.h"
#include "BlockingQueue.h"
#include "Communication.h"
#include "Interval.h"
#include "Motion.h"

namespace motion_classifier {

	class Classifier {
	protected:
		BlockingQueue<Motion::Muscle*> lastMuscleMotion{ 1, AppConfig::getInstance()->getBlockingQueueMaxWaitTime() };
		AppConfig *config;
		Status status = NEW;

		//only needed for statistics
		int intervalCount = 0;
		long time = 0;

	public:

		/**
	  	  * Returns the last stored MuscleMotion. The stored value is
		  * deleted by this operation. When no MuscleMotion is
		  * available, the calling Thread will be blocked until an new
		  * MuscleMotion is available. When a maximum waiting time is
		  * defined and the timeout is reached UNKNOWN will be returned.
		  */
		virtual Motion::Muscle getMuscleMotion() = NULL;

		/**
		  * Sends the given signal to the underlying worker Thread.
		  */
		virtual void send(const Signal& signal) = NULL;

		/**
		  * Returns statistic information about the classification process
		  * as a std::string.
		  */
		virtual std::string printStatistics() = NULL;

	};
}

#endif