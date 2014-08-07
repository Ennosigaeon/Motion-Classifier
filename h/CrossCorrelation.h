
#ifndef CROSSCORRELATION_H
#define CROSSCORRELATION_H

#include "ClassifierImpl.h"
#include "Logger.h"

namespace motion_classifier {

	class CrossCorrelation {

	private:
		Logger *logger;
		ClassifierImpl *classifier;

	public:
		CrossCorrelation(ClassifierImpl *classifier);

		void testClassifier(std::map<Motion::Muscle, std::vector<Interval*>*>* data);

		std::map<Motion::Muscle, std::vector<Interval*>*>* loadData(std::string folder, int size);
	};
}

#endif