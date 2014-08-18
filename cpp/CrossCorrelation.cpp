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

std::map<Motion::Muscle, std::vector<Interval*>*>* CrossCorrelation::loadData(std::string folder, int size) {
	auto data = new std::map < Motion::Muscle, std::vector<Interval*>* > ;

	std::array<std::string, 9> movements = { "pronation", "supination", "extension", "flexion", "open", "close", "rest", "ulnar", "radial" };
	for (auto &movement : movements) {
		std::vector<Interval*> *list = new std::vector < Interval* > ;
		for (int i = 1; i < 5; ++i) {
			std::ifstream in(folder + movement + "-" + boost::lexical_cast<std::string>(i)+".txt");
			//Skip intervals
			for (int j = 0; j < 14 - size; ++j) {
				std::string s;
				std::getline(in, s);
			}
			for (int j = 0; j < size * 2 + 1; ++j) {
				Interval *interval = new Interval;
				Sample *s = new Sample;
				in >> *s;
				interval->addSample(s);
				list->push_back(interval);
			}
		}
		data->insert(std::make_pair(motion_classifier::getMotion(movement), list));
	}
	return data;
}