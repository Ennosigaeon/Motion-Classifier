#ifndef CLASSIFIERIMPL_H
#define CLASSIFIERIMPL_H

#include <condition_variable>
#include <mutex>
#include <thread>

#include "Motion.h"
#include "Classifier.h"
#include "EMGProvider.h"
#include "Logger.h"

namespace motion_classifier {

	class ClassifierImpl : public Classifier {
		friend class CrossCorrelation;

	protected:
		BlockingQueue<Motion::Muscle*> lastMuscleMotion{ 1, AppConfig::getInstance()->getBlockingQueueMaxWaitTime() };
		Status status = NEW;
		Logger *logger = Logger::getInstance();
		EMGProvider *emgProvider;
		AppConfig *config;

		std::thread worker;
		std::mutex mutex;
		std::condition_variable condition;

		//only needed for statistics
		int intervalCount = 0;
		long time = 0;
		virtual Motion::Muscle classify(Interval *interval) = NULL;
		void run();

	public:


		//TODO: remove
		int _A;
		int _B;
		int _C;


		virtual std::string printStatistics();
		virtual Motion::Muscle getMuscleMotion();
		virtual void send(const Signal& signal);
	};
}

#endif