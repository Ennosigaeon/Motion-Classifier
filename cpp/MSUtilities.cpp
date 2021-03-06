#include <boost/lexical_cast.hpp>
#include <fstream>
#include "../h/MSUtilities.h"
#include "../h/Utilities.h"

#include "../h/MSClassifier.h"

using namespace motion_classifier;

MSUtilities::MSUtilities(MSClassifier *classifier) {
	MSUtilities::classifier = classifier;
	logger = Logger::getInstance();
}

double MSUtilities::crossCorrelation(std::map<Motion::Muscle, std::vector<Interval*>*> *train, std::map<Motion::Muscle, std::vector<Interval*>*> *test, int count) {
	double avg = 0;
	for (int i = 1; i < count; ++i) {
		std::map<Motion::Muscle, std::vector<Interval*>*> trainMap;
		for (const auto &pair : *train) {
			std::vector<Interval*> *trainList = new std::vector < Interval* > ;
			for (const auto &interval : *pair.second)
				if (interval->getGroup() != i)
					trainList->push_back(interval);
			trainMap.insert(std::make_pair(pair.first, trainList));
		}
		classifier->train(&trainMap);

		std::map<Motion::Muscle, std::vector<Interval*>*> testMap;
		for (const auto &pair : *test) {
			std::vector<Interval*> *testList = new std::vector < Interval* > ;
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
	}
	avg /= count - 1;
	//std::cout << "Average: " << avg;

	return avg;
}


double MSUtilities::testClassifier(std::map<Motion::Muscle, std::vector<Interval*>*>* data) {
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

void MSUtilities::testElectrodeLost(DirProvider *provider, int count, int start, int end) {
	std::ofstream out("C:/Tmp/lost.txt");
	std::array<std::vector<double>, 100> result;
	for (int i = 0; i < count; i++) {
		int n = 0;
		for (double d = 0; d < 1; d += 0.01) {
			logger->info("Setting electrode lost to " + boost::lexical_cast<std::string>(d));

			provider->setElectrodeLost(d);
			auto test = provider->getIntervalSubset(start, end);

			double res = testClassifier(test);
			if (i == 0)
				result[n] = std::vector < double >();
			result[n].push_back(res);

			out << d << "\t" << res << std::endl;

			provider->releaseIntervalSubset(test);
		}
	}

	std::ofstream out2("C:/Tmp/electrode_lost.txt");
	double lost = 0;
	for (const auto &list : result) {
		double avg = 0;
		for (const auto &d : list)
			avg += d;
		avg /= list.size();

		double tmp = 0;
		for (const auto &d : list)
			tmp += (d - avg) * (d - avg);
		double deviation = sqrt(1 / (list.size() - 1) * tmp);

		out2 << lost << "\t" << avg << "\t" << deviation << std::endl;
		lost += 0.01;
	}

}


void MSUtilities::findAllParameters(std::map<Motion::Muscle, std::vector<Interval*>*>* train, std::map < Motion::Muscle, std::vector<Interval*>*>* test, int nr) {
	logger->info("Searching for optimal parameters...");
	std::ofstream out("C:/Tmp/parameters-" + boost::lexical_cast<std::string>(nr)+".txt");
	std::vector<Entry> entries;

	int a[4] = { 1, 2, 4, 8 };
	int b[7] = { 2, 3, 4, 6, 8, 12, 24 };
	int c[11] = { 2, 4, 5, 8, 10, 16, 20, 25, 40, 50, 80 };
	int h[13] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

	int startA = 0;
	int endA = 4;
	if (nr == 0)
		endA = 2;
	if (nr == 1)
		startA = 2;

	for (int i = startA; i < endA; ++i) {
		for (int j = 0; j < 7; ++j) {
			for (int k = 0; k < 11; ++k) {
				classifier->m = a[i];
				classifier->n = b[j];
				classifier->p = c[k];

				for (int l = 7; l < 8; ++l) {
					classifier->h = h[l];

					double result = crossCorrelation(train, test, 5);
					std::cout << classifier->h << "\t" << classifier->m << "\t" << classifier->n << "\t" << classifier->p << "\t" << result << std::endl;
					out << classifier->h << "\t" << classifier->m << "\t" << classifier->n << "\t" << classifier->p << "\t" << result << std::endl;

					entries.push_back({ classifier->h, classifier->m, classifier->n, classifier->p, result });
				}
			}
		}
	}
	out.close();

	std::sort(entries.begin(), entries.end(), [](const Entry &left, const Entry &right) {
		return left.error < right.error;
	});
	std::ofstream out2("C:/Tmp/ordered_parameters-" + boost::lexical_cast<std::string>(nr)+".txt");
	for (const auto &entry : entries)
		out2 << entry.h << "\t" << entry.x << "\t" << entry.y << "\t" << entry.z << "\t" << entry.error << std::endl;
	out2.close();
}
