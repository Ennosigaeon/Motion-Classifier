
#ifndef CROSSCORRELATION_H
#define CROSSCORRELATION_H

#include "DirProvider.h"
#include "MSClassifier.h"
#include "Logger.h"

namespace motion_classifier {

	struct Entry {
		int x;
		int y;
		int z;
		double error;
	};


	class CrossCorrelation {

	private:
		Logger *logger;
		MSClassifier *classifier;

	public:
		CrossCorrelation(MSClassifier *classifier);

		double testClassifier(std::map<Motion::Muscle, std::vector<Interval*>*>* data);

		void testElectrodeLost(DirProvider *provider);

		void findParameter(std::map<Motion::Muscle, std::vector<Interval*>*>* data);
	};
}

#endif