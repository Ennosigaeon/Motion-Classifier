#include <boost/lexical_cast.hpp>
#include <fstream>
#include "../h/CrossCorrelation.h"
#include "../h/Utilities.h"

using namespace motion_classifier;

CrossCorrelation::CrossCorrelation(ClassifierImpl *classifier) {
	CrossCorrelation::classifier = classifier;
	logger = Logger::getInstance();
}

void CrossCorrelation::testClassifier(std::map<Motion::Muscle, std::vector<Interval*>*>* data) {
	logger->info("Starting cross correlation...");

	int a[4] = { 1, 2, 4, 8 };
	int b[8] = { 1, 2, 3, 4, 6, 8, 12, 24 };
	int c[15] = { 1, 2, 4, 5, 8, 10, 16, 20, 25, 40, 50, 80, 100, 200, 400 };


	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 8; ++j) {
			for (int k = 0; k < 15; ++k) {
				int totalFailed = 0, total = 0;
				for (auto &pair : *data) {
					int i = 0, failed = 0;
					for (auto &interval : *pair.second) {

						classifier->_A = a[i];
						classifier->_B = b[j];
						classifier->_C = c[k];

						Motion::Muscle motion = classifier->classify(interval);
						if (motion != pair.first) {
							logger->debug("Result is wrong: Interval " + boost::lexical_cast<std::string>(i)+" was classified as " + printMotion(motion)
								+ " but was expected to be " + printMotion(pair.first));
							failed++;
						}
						i++;
					}
					logger->info(printMotion(pair.first) + ": " + boost::lexical_cast<std::string>(pair.second->size() - failed) + " successfull, "
						+ boost::lexical_cast<std::string>(failed)+" failed");
					totalFailed += failed;
					total += pair.second->size();
				}
				logger->info(boost::lexical_cast<std::string>(totalFailed)+" of " + boost::lexical_cast<std::string>(total)+" failed: " +
					boost::lexical_cast<std::string>(totalFailed * 1.0 / total));

				std::cout << "\t" << a[i] << "\t" << b[j] << "\t" << c[k] << totalFailed * 1.0 / total << std::endl;
			}
		}
	}
}

std::map<Motion::Muscle, std::vector<Interval*>*>* CrossCorrelation::loadData(std::string folder, int size) {
	auto data = new std::map<Motion::Muscle, std::vector<Interval*>*>;

	std::array<std::string, 9> movements = { "pronation", "supination", "extension", "flexion", "open", "close", "rest", "ulnar", "radial" };
	for (auto &movement : movements) {
		std::vector<Interval*> *list = new std::vector < Interval* >;
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