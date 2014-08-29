
#ifndef CROSSCORRELATION_H
#define CROSSCORRELATION_H

#include "DirProvider.h"
#include "MSClassifier.h"
#include "Logger.h"

namespace motion_classifier {

	struct Entry {
		int h;
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

		void findSpaceParameter(std::map<Motion::Muscle, std::vector<Interval*>*>* data);

		void findBandwidth(std::map<Motion::Muscle, std::vector<Interval*>*>* data);

		void findAllParameters(std::map < Motion::Muscle, std::vector<Interval*>*>* train, std::map < Motion::Muscle, std::vector<Interval*>*>* test, int nr);
	};
}

#endif