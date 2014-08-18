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
		void run();

	public:

		virtual std::string printStatistics();
		virtual Motion::Muscle getMuscleMotion();
		virtual void send(const Signal& signal);
	};
}

#endif