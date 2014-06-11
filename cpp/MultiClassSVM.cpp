#include <ctime>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/MultiClassSVM.h"
#include "../h/Trainer.h"
#include "../h/Utilities.h"
#include "../h/Variogram.h"

MultiClassSVM::~MultiClassSVM() {
	for (std::vector<SupportVectorMachine*>::iterator it = svms.begin(); it != svms.end(); ++it)
		delete *it;
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
	int offset = 0;
	for (std::map<Motion::Muscle, std::vector<math::Vector>>::iterator it = trainingsData.begin(); it != trainingsData.end(); ++it) {
		offset++;
		if (it->second.empty())
			continue;

		int i = 0;
		for (std::map<Motion::Muscle, std::vector<math::Vector>>::iterator it2 = trainingsData.begin(); it2 != trainingsData.end(); it2++) {
			if (i++ < offset || it2->second.empty())
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

	//TODO: Do not use Magic Numbers
	int count[9];
	for (int i = 0; i < 9; ++i)
		count[i] = 0;

	int length = sizeof(count) / sizeof(count[0]);
	for (std::vector<SupportVectorMachine*>::iterator it = svms.begin(); it != svms.end(); ++it) {
		Motion::Muscle result = (*it)->classify(values);
		if (result < 9)
			++count[result];
	}

	int max = 0;
	for (int i = 0; i < 9; ++i) {
		if (count[i] > max) {
			max = count[i];
			motion = static_cast<Motion::Muscle>(i);
		}
	}

	t = clock() - t;
	BOOST_LOG_TRIVIAL(debug) << "classification took " << ((double)t) / CLOCKS_PER_SEC * 1000 << " ms";

	return motion;
}