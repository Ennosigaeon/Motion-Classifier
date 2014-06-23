#include <ctime>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/MultiClassSVM.h"
#include "../h/Trainer.h"
#include "../h/Utilities.h"
#include "../h/Variogram.h"

using namespace motion_classifier;

MultiClassSVM::MultiClassSVM(Properties *prop) {
	param = new svm_parameter;
	//These default values are copied from svm_train.cpp from libsvm-3.18
	param->svm_type = prop->getInt("svm.type");
	param->kernel_type = prop->getInt("svm.kernel");
	param->degree = 3;
	param->gamma = 0;
	param->coef0 = 0;
	param->nu = 0.5;
	param->cache_size = 100;
	param->C = prop->getInt("svm.cost");
	param->eps = 1e-3;
	param->p = 0.1;
	param->shrinking = 1;
	param->probability = 0;
	param->nr_weight = 0;
	param->weight_label = NULL;
	param->weight = NULL;
}

MultiClassSVM::~MultiClassSVM() {
	for (std::vector<SupportVectorMachine*>::iterator it = svms.begin(); it != svms.end(); ++it)
		delete *it;
	delete param;
}

void MultiClassSVM::train(const Motion::Muscle& motion, const std::vector<math::Vector>& data) {
	BOOST_LOG_TRIVIAL(trace) << "adding trainings data for MuscleMotion " << motion;
	std::map<Motion::Muscle, std::vector<math::Vector>>::iterator search = trainingsData.find(motion);
	if (search == trainingsData.end()) {
		std::vector<math::Vector> vector(data);
		trainingsData.insert(std::make_pair(motion, vector));
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
			SupportVectorMachine *svm = new SupportVectorMachine(param);
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

	int count[Motion::Muscle::HAND_CLOSE];
	for (int i = 0; i < Motion::Muscle::HAND_CLOSE; ++i)
		count[i] = 0;

	int length = sizeof(count) / sizeof(count[0]);
	for (std::vector<SupportVectorMachine*>::iterator it = svms.begin(); it != svms.end(); ++it) {
		Motion::Muscle result = (*it)->classify(values);
		++count[result];
	}

	int max = 0;
	for (int i = 0; i < Motion::Muscle::HAND_CLOSE; ++i) {
		if (count[i] > max) {
			max = count[i];
			motion = static_cast<Motion::Muscle>(i);
		}
	}

	t = clock() - t;
	BOOST_LOG_TRIVIAL(debug) << "classification took " << ((double)t) / CLOCKS_PER_SEC * 1000 << " ms";

	return motion;
}