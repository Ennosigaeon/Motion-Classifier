#include <ctime>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/MultiClassSVM.h"

MultiClassSVM::~MultiClassSVM() {
	for (std::vector<SupportVectorMachine*>::iterator it = svms.begin(); it != svms.end(); it++)
		delete *it;
}

void MultiClassSVM::train(const MuscleMotion& motion, const std::vector<math::Vector>& data) {
	BOOST_LOG_TRIVIAL(trace) << "adding trainings data for MuscleMotion " << motion;
	std::map<MuscleMotion, std::vector<math::Vector>>::iterator search = trainingsData.find(motion);
	if (search == trainingsData.end()) {
		std::vector<math::Vector> vector(data);
		trainingsData.insert(std::pair<MuscleMotion, std::vector<math::Vector>>(motion, vector));
	}
	else {
		std::vector<math::Vector> *vector = &(search->second);
		vector->insert(vector->end(), data.begin(), data.end());
	}
}

void MultiClassSVM::calculateSVMs() {
	for (std::map<MuscleMotion, std::vector<math::Vector>>::iterator it = trainingsData.begin(); it != trainingsData.end(); it++) {
		for (std::map<MuscleMotion, std::vector<math::Vector>>::iterator it2 = trainingsData.begin(); it2 != trainingsData.end(); it2++) {
			if (it->first == it2->first)
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

MuscleMotion MultiClassSVM::classify(std::vector<math::Vector>& values) {
	MuscleMotion motion = MuscleMotion::UNKNOWN;
	clock_t t = clock();

	int count[MuscleMotion::HAND_CLOSE] = {};
	int length = sizeof(count) / sizeof(count[0]);
	for (std::vector<SupportVectorMachine*>::iterator it = svms.begin(); it != svms.end(); it++) {
		MuscleMotion result = (*it)->classify(values);
		count[result]++;
	}

	int max = 0;
	for (int i = 0; i < MuscleMotion::HAND_CLOSE; i++) {
		if (count[i] > max) {
			max = count[i];
			motion = static_cast<MuscleMotion>(i);
		}
	}

	t = clock() - t;
	BOOST_LOG_TRIVIAL(debug) << "classification took " << ((double)t) / CLOCKS_PER_SEC * 1000 << " ms";

	return motion;
}