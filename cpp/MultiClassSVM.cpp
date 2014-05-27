#include <ctime>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/MultiClassSVM.h"
#include "../h/Trainer.h"
#include "../h/Variogram.h"

MultiClassSVM::~MultiClassSVM() {
	for (std::vector<SupportVectorMachine*>::iterator it = svms.begin(); it != svms.end(); it++)
		delete *it;
}

void MultiClassSVM::train(const std::string& folder) {
	AppConfig *config = AppConfig::getInstance();
	Variogram variogram;
	BOOST_LOG_TRIVIAL(info) << "loading trainings data from " << folder;
	for (int i = Motion::Muscle::REST_POSITION; i <= Motion::Muscle::HAND_CLOSE; i++) {
		std::string file = folder;
		switch (i) {
		case Motion::Muscle::REST_POSITION:
			file += "REST_POSITION";
			break;
		case Motion::Muscle::FORARM_PRONATION:
			file += "FORARM_PRONATION";
			break;
		case Motion::Muscle::FORARM_SUPINATION:
			file += "FORARM_SUPINATION";
			break;
		case Motion::Muscle::WRIST_FLEXION:
			file += "WRIST_FLEXION";
			break;
			case Motion::Muscle::WRIST_EXTENSION:
			file += "WRIST_EXTENSION";
			break;
		case Motion::Muscle::HAND_OPEN:
			file += "HAND_OPEN";
			break;
		case Motion::Muscle::HAND_CLOSE:
			file += "HAND_CLOSE";
			break;
		case Motion::Muscle::RADIAL_DEVIATION:
			file += "RADIAL_DEVIATION";
			break;
		case Motion::Muscle::ULNAR_DEVIATION:
			file += "ULNAR_DEVIATION";
			break;
		}
		file += "-";

		std::vector<math::Vector> result;
		for (int j = 0; j < Trainer::NR_RUNS; j++) {
			std::string tmp = file;
			tmp += boost::lexical_cast<std::string>(j)+".txt";
			std::ifstream in(tmp);
			//This trainigs file does not exists
			if (!in.is_open())
				continue;
			for (int n = 0; n < Trainer::trainingsSize; n++) {
				Sample s(config->getSampleRows(), config->getSampleColumns());
				in >> s;
				std::vector<math::Vector> r = variogram.calculate(&s);
				result.insert(result.end(), r.begin(), r.end());
			}
		}
		train(static_cast<Motion::Muscle>(i), result);
	}
}

void MultiClassSVM::train(const Motion::Muscle& motion, const std::vector<math::Vector>& data) {
	BOOST_LOG_TRIVIAL(trace) << "adding trainings data for MuscleMotion " << motion;
	std::map<Motion::Muscle, std::vector<math::Vector>>::iterator search = trainingsData.find(motion);
	if (search == trainingsData.end()) {
		std::vector<math::Vector> vector(data);
		trainingsData.insert(std::pair<Motion::Muscle, std::vector<math::Vector>>(motion, vector));
	}
	else {
		std::vector<math::Vector> *vector = &(search->second);
		vector->insert(vector->end(), data.begin(), data.end());
	}
}

void MultiClassSVM::calculateSVMs() {
	for (std::map<Motion::Muscle, std::vector<math::Vector>>::iterator it = trainingsData.begin(); it != trainingsData.end(); it++) {
		for (std::map<Motion::Muscle, std::vector<math::Vector>>::iterator it2 = trainingsData.begin(); it2 != trainingsData.end(); it2++) {
			if (it->first == it2->first || it->second.empty() || it2->second.empty())
				continue;
			SupportVectorMachine *svm = new SupportVectorMachine;
			svm->addTrainData(it->first, it->second);
			svm->addTrainData(it2->first, it2->second);
			svm->calculateSVM();
			svms.push_back(svm);
		}
	}
	BOOST_LOG_TRIVIAL(info) << "Calculated " << svms.size() << " Support Vector Machines for " << trainingsData.size() << " MuscleMotions.";
}

Motion::Muscle MultiClassSVM::classify(std::vector<math::Vector>& values) {
	Motion::Muscle motion = Motion::Muscle::UNKNOWN;
	clock_t t = clock();

	int count[9];
	for (int i = 0; i < 9; i++)
		count[i] = 0;

	int length = sizeof(count) / sizeof(count[0]);
	for (std::vector<SupportVectorMachine*>::iterator it = svms.begin(); it != svms.end(); it++) {
		Motion::Muscle result = (*it)->classify(values);
		if (result < 9)
			count[result]++;
	}

	int max = 0;
	for (int i = 0; i < 9; i++) {
		if (count[i] > max) {
			max = count[i];
			motion = static_cast<Motion::Muscle>(i);
		}
	}

	t = clock() - t;
	BOOST_LOG_TRIVIAL(debug) << "classification took " << ((double)t) / CLOCKS_PER_SEC * 1000 << " ms";

	return motion;
}