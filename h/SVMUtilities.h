
#ifndef SVMUTILITIES_H
#define SVMUTILITIES_H

#include "DirProvider.h"
#include "SVMClassifier.h"
#include "Logger.h"

namespace motion_classifier {

	class SVMUtilities {

	private:
		Logger *logger;
		SVMClassifier *classifier;

	public:
		SVMUtilities(SVMClassifier *classifier);

		double crossCorrelation(std::map<Motion::Muscle, std::vector<Interval*>*> *train, std::map<Motion::Muscle, std::vector<Interval*>*> *test, int count);

		double testClassifier(std::map<Motion::Muscle, std::vector<Interval*>*>* data);

	};
}

#endif