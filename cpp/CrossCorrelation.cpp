#include <boost/lexical_cast.hpp>
#include <fstream>
#include "../h/CrossCorrelation.h"
#include "../h/Utilities.h"

#include "../h/MSClassifier.h"

using namespace motion_classifier;

CrossCorrelation::CrossCorrelation(MSClassifier *classifier) {
	CrossCorrelation::classifier = classifier;
	logger = Logger::getInstance();
}

double CrossCorrelation::testClassifier(std::map<Motion::Muscle, std::vector<Interval*>*>* data) {
	logger->info("Starting cross correlation...");
	std::vector<Entry> entries;

	int totalFailed = 0, total = 0;
	for (auto &pair : *data) {
		int n = 0, failed = 0;
		for (auto &interval : *pair.second) {
			classifier->calcTrainingsMatrix();

			Motion::Muscle motion = classifier->classify(interval);
			if (motion != pair.first) {
				logger->trace("Result is wrong: Interval " + boost::lexical_cast<std::string>(n)+" was classified as " + printMotion(motion)
					+ " but was expected to be " + printMotion(pair.first));
				failed++;
			}
			n++;
		}
		logger->debug(printMotion(pair.first) + ": " + boost::lexical_cast<std::string>(pair.second->size() - failed) + " successfull, "
			+ boost::lexical_cast<std::string>(failed)+" failed");
		totalFailed += failed;
		total += pair.second->size();
	}
	logger->info(boost::lexical_cast<std::string>(totalFailed)+" of " + boost::lexical_cast<std::string>(total)+" failed: " +
		boost::lexical_cast<std::string>(totalFailed * 1.0 / total));

	double result = totalFailed * 1.0 / total;
	std::cout << classifier->m << "\t" << classifier->n << "\t" << classifier->p << "\t" << result << std::endl;
	return result;
}

void CrossCorrelation::testElectrodeLost(DirProvider *provider) {
	std::array<double, 20> results;
	for (int i = 0; i < 5; i++) {
		int n = 0;
		for (double d = 0; d < 1; d += 0.05) {
			logger->info("Setting electrode lost to " + boost::lexical_cast<std::string>(d));

			provider->setElectrodeLost(d);
			auto test = provider->getIntervalSubset(14, 15);

			double res = testClassifier(test);
			if (i == 0)
				results[n] = res;
			else
				results[n] += res;
			++n;

			provider->releaseIntervalSubset(test);
		}
	}

	std::ofstream out("C:/Tmp/electrode_lost.txt");
	double d = 0;
	for (double r : results) {
		out << d << "\t" << r / 5 << std::endl;
		d += 0.05;
	}
	out.close();
}

void CrossCorrelation::findParameter(std::map<Motion::Muscle, std::vector<Interval*>*>* data) {
	logger->info("Searching for optimal parameters...");
	std::string file = "C:/Tmp/parameters.txt";
	std::vector<Entry> entries;

	int a[4] = { 1, 2, 4, 8 };
	int b[8] = { 1, 2, 3, 4, 6, 8, 12, 24 };
	int c[15] = { 1, 2, 4, 5, 8, 10, 16, 20, 25, 40, 50, 80, 100, 200, 400 };

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 8; ++j) {
			for (int k = 0; k < 15; ++k) {
				classifier->m = a[i];
				classifier->n = b[j];
				classifier->p = c[k];
				double result = testClassifier(data);
				entries.push_back({ a[i], b[j], c[k], result });
			}
		}
	}

	std::ofstream out("C:/Tmp/parameters.txt");
	for (const auto &entry : entries)
		out << entry.x << "\t" << entry.y << "\t" << entry.z << "\t" << entry.error << std::endl;
	out.close();

	std::sort(entries.begin(), entries.end(), [](const Entry &left, const Entry &right) {
		return left.error < right.error;
	});

	std::ofstream out2("C:/Tmp/ordered_parameters.txt");
	for (const auto &entry : entries)
		out2 << entry.x << "\t" << entry.y << "\t" << entry.z << "\t" << entry.error << std::endl;
	out2.close();
}