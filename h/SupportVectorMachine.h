#ifndef SUPPORT_VECTOR_MACHINE_H
#define SUPPORT_VECTOR_MACHINE_H

#include <map>
#include <vector>
#include "../lib/libsvm-3.18/svm.h"
#include "AppConfig.h"
#include "Motion.h"
#include "Vector.h"

class SupportVectorMachine {
private:
	Motion::Muscle classA = Motion::Muscle::UNKNOWN;
	Motion::Muscle classB = Motion::Muscle::UNKNOWN;
	std::vector<math::Vector> valuesA;
	std::vector<math::Vector> valuesB;
	AppConfig *config;
	svm_model *svm;
	svm_problem problem;

	svm_node* createNode(math::Vector* vector);

public:
	SupportVectorMachine();
	~SupportVectorMachine();

	void addTrainData(const Motion::Muscle& motion, std::vector<math::Vector>& values);
	void calculateSVM();
	Motion::Muscle classify(std::vector<math::Vector>& values);
};

#endif