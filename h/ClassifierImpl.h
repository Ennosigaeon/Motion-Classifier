#ifndef CLASSIFIERIMPL_H
#define CLASSIFIERIMPL_H

#include "Motion.h"
#include "Classifier.h"

namespace motion_classifier {

	class ClassifierImpl : public Classifier {
	protected:
		BlockingQueue<Motion::Muscle*> lastMuscleMotion{ 1, AppConfig::getInstance()->getBlockingQueueMaxWaitTime() };
		Status status = NEW;
		AppConfig *config;

		//only needed for statistics
		int intervalCount = 0;
		long time = 0;

	public:
		virtual std::string printStatistics();
		virtual Motion::Muscle getMuscleMotion();

	};
}

#endif