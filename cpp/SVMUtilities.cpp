#include <boost/lexical_cast.hpp>
#include <fstream>
#include "../h/SVMUtilities.h"
#include "../h/Utilities.h"


using namespace motion_classifier;

SVMUtilities::SVMUtilities(SVMClassifier *classifier) {
	SVMUtilities::classifier = classifier;
	logger = Logger::getInstance();
}

double SVMUtilities::crossCorrelation(std::map<Motion::Muscle, std::vector<Interval*>*> *train, std::map<Motion::Muscle, std::vector<Interval*>*> *test, int count) {
	double avg = 0;
	for (int i = 1; i < count; ++i) {
		std::map<Motion::Muscle, std::vector<Interval*>*> trainMap;
		for (const auto &pair : *train) {
			std::vector<Interval*> *trainList = new std::vector < Interval* >;
			for (const auto &interval : *pair.second)
				if (interval->getGroup() != i)
					trainList->push_back(interval);
			trainMap.insert(std::make_pair(pair.first, trainList));
		}
		classifier->train(&trainMap);

		std::map<Motion::Muscle, std::vector<Interval*>*> testMap;
		for (const auto &pair : *test) {
			std::vector<Interval*> *testList = new std::vector < Interval* >;
			for (const auto &interval : *pair.second)
				if (interval->getGroup() == i)
					testList->push_back(interval);
			testMap.insert(std::make_pair(pair.first, testList));
		}
		avg += testClassifier(&testMap);

		for (auto &pair : trainMap)
			delete pair.second;
		for (auto &pair : testMap)
			delete pair.second;
		classifier->svm->reset();
	}
	avg /= count - 1;
	//std::cout << "Average: " << avg;

	return avg;
}


double SVMUtilities::testClassifier(std::map<Motion::Muscle, std::vector<Interval*>*>* data) {
	logger->info("Starting cross correlation...");

	int totalFailed = 0, total = 0;
	for (auto &pair : *data) {
		int failed = 0;
		for (const auto const &interval : *pair.second) {
			Motion::Muscle motion = classifier->classify(interval);
			if (motion != pair.first)
				failed++;
		}
		logger->debug(printMotion(pair.first) + ": " + boost::lexical_cast<std::string>(pair.second->size() - failed) + " successfull, "
			+ boost::lexical_cast<std::string>(failed)+" failed");
		totalFailed += failed;
		total += pair.second->size();
	}
	logger->info(boost::lexical_cast<std::string>(totalFailed)+" of " + boost::lexical_cast<std::string>(total)+" failed: " +
		boost::lexical_cast<std::string>(totalFailed * 1.0 / total));

	double result = totalFailed * 1.0 / total;
	//std::cout << classifier->h << "\t" << classifier->m << "\t" << classifier->n << "\t" << classifier->p << "\t" << result << std::endl;
	return result;
}