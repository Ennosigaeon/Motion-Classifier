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
	std::cout << classifier->h << "\t" << classifier->m << "\t" << classifier->n << "\t" << classifier->p << "\t" << result << std::endl;
	return result;
}

void CrossCorrelation::testElectrodeLost(DirProvider *provider, int count, int start, int end) {
	std::array<double, 20> results;
	for (int i = 0; i < count; i++) {
		int n = 0;
		for (double d = 0; d < 1; d += 0.05) {
			logger->info("Setting electrode lost to " + boost::lexical_cast<std::string>(d));

			provider->setElectrodeLost(d);
			auto test = provider->getIntervalSubset(start, end);

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
		out << d << "\t" << r / count << std::endl;
		d += 0.05;
	}
	out.close();
}


void CrossCorrelation::findAllParameters(std::map<Motion::Muscle, std::vector<Interval*>*>* train, std::map < Motion::Muscle, std::vector<Interval*>*>* test, int nr) {
	logger->info("Searching for optimal parameters...");
	std::ofstream out("C:/Tmp/parameters-" + boost::lexical_cast<std::string>(nr)+".txt");
	std::vector<Entry> entries;

	int a[4] = { 1, 2, 4, 8 };
	int b[8] = { 1, 2, 3, 4, 6, 8, 12, 24 };
	int c[11] = { 2, 4, 5, 8, 10, 16, 20, 25, 40, 50, 80 };
	int h[13] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

	int startA = 0;
	int endA = 4;
	if (nr == 0)
		endA = 2;
	if (nr == 1)
		startA = 2;

	for (int i = startA; i < endA; ++i) {
		for (int j = 0; j < 8; ++j) {
			for (int k = 0; k < 11; ++k) {
				classifier->m = a[i];
				classifier->n = b[j];
				classifier->p = c[k];
				
				for (int l = 0; l < 13; ++l) {
					classifier->h = h[l];
					classifier->train(train);

					double result = testClassifier(test);
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
