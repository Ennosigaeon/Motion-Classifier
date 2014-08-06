#include "../h/Logger.h"
#include "../h/MSClassifier.h"

#include <array>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include "../h/Utilities.h"

#define NR_INTERVALS 1

using namespace motion_classifier;

MSClassifier::MSClassifier(EMGProvider *emg, Properties *configuration) {
	h = configuration->getDouble("ms.h");

	double minX = configuration->getDouble("space.x.min"), maxX = configuration->getDouble(""), minY = configuration->getDouble(""),
		maxY = configuration->getDouble("");
	if (isnan(minX))
		minX = -std::numeric_limits<double>::infinity();
	if (isnan(maxX))
		maxX = std::numeric_limits<double>::infinity();
	if (isnan(minY))
		minY = -std::numeric_limits<double>::infinity();
	if (isnan(maxY))
		maxY = std::numeric_limits<double>::infinity();
	math::Space *space = new math::Space(minX, maxX, minY, maxY);
	space->setNorm(static_cast<math::Norm>(configuration->getInt("space.norm")));
	
	msAlgo = &MeanShift(static_cast<math::KernelType>(configuration->getInt("ms.kernel")), space, configuration->getDouble("ms.epsilon"),
		configuration->getDouble("ms.threshold"));
	msAlgo->setFiltering(configuration->getBool("ms.filtering"));
}

MSClassifier::~MSClassifier() {
	for (auto pair : trainingsData) {
		for (auto vector : *pair.second)
			delete vector;
		delete pair.second;
	}
	math::Vector::setSpace(new math::Space);
}

void MSClassifier::run() {
	Logger *logger = Logger::getInstance();
	while (true) {
		if (status == Status::RUNNING) {

		}
		if (status == Status::WAITING) {

		}
		if (status == Status::FINISHED) {
			logger->info("shutting down MSClassifier worker");
			return;
		}
	}
}

void MSClassifier::send(const Signal& signal) {

}

void MSClassifier::train(std::map<Motion::Muscle, std::vector<Interval*>*>* training) {
	for (auto pair : *training) {
		std::vector<math::Vector*> *allCenters = new std::vector < math::Vector* > ;
		for (Interval *interval : *pair.second) {
			Sample *mean = interval->getMeanSample();
			msAlgo->setDataPoints(mean->getEntries());
			std::vector<math::Vector*> *centers = msAlgo->calculate(h);
			allCenters->insert(allCenters->end(), centers->begin(), centers->end());
		}

		auto it = allCenters->begin();
		while (it != allCenters->end()) {
			if ((**it).getGroup() == -1) {
				it = allCenters->erase(it);
				continue;
			}

			auto it2 = it;
			it2++;
			for (; it2 != allCenters->end(); ++it2) {
				if ((**it).getDistance(**it2) < h) {
					**it += **it2;
					**it /= 2;
					(**it2).setGroup(-1);
				}
			}
			it++;
		}

		trainingsData.insert(std::make_pair(pair.first, allCenters));
	}
	msAlgo->setDataPoints(NULL);
}

std::map<Motion::Muscle, std::vector<math::Vector*>*>* MSClassifier::getTrainingsData() {
	return &trainingsData;
}

//This function is only used to extract data from the HDD and convert them into the right format. HAS TO BE REMOVED!!!
std::map< Motion::Muscle, std::vector<Interval*>*>* MSClassifier::extractTrainingsData(std::string folder) {
	auto data = new std::map<Motion::Muscle, std::vector<Interval*>*>;

	std::array<std::string, 9> movements = { "pronation", "supination", "extension", "flexion", "open", "close", "rest", "ulnar", "radial" };
	for (auto &movement : movements) {
		std::vector<Interval*> *list = new std::vector < Interval* >;
		for (int i = 1; i < 5; ++i) {
			std::ifstream in(folder + movement + "-" + boost::lexical_cast<std::string>(i)+".txt");
			//Skip intervals
			for (int j = 0; j < 14 - NR_INTERVALS; ++j) {
				std::string s;
				std::getline(in, s);
			}
			for (int j = 0; j < NR_INTERVALS * 2 + 1; ++j) {
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