
#ifndef MSUTILITIES_H
#define MSUTILITIES_H

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


	class MSUtilities {

	private:
		Logger *logger;
		MSClassifier *classifier;

	public:
		MSUtilities(MSClassifier *classifier);

		double crossCorrelation(std::map<Motion::Muscle, std::vector<Interval*>*> *train, std::map<Motion::Muscle, std::vector<Interval*>*> *test, int count);

		double testClassifier(std::map<Motion::Muscle, std::vector<Interval*>*>* data);

		void testElectrodeLost(DirProvider *provider, int count, int start, int end);

		void findAllParameters(std::map < Motion::Muscle, std::vector<Interval*>*>* train, std::map < Motion::Muscle, std::vector<Interval*>*>* test, int nr);
	};
}

#endif