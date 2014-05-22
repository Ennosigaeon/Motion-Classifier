#ifndef SUPPORT_VECTOR_MACHINE_H
#define SUPPORT_VECTOR_MACHINE_H

#include <map>
#include <vector>
#include "../lib/libsvm-3.18/svm.h"
#include "AppConfig.h"
#include "MuscleMotion.h"
#include "Vector.h"

class SupportVectorMachine {
private:
	MuscleMotion classA = MuscleMotion::UNKNOWN;
	MuscleMotion classB = MuscleMotion::UNKNOWN;
	std::vector<math::Vector> valuesA;
	std::vector<math::Vector> valuesB;
	AppConfig *config;
	svm_model *svm;
	svm_problem problem;

	svm_node* createNode(math::Vector* vector);

public:
	SupportVectorMachine();
	~SupportVectorMachine();

	void addTrainData(const MuscleMotion& motion, std::vector<math::Vector>& values);
	void calculateSVM();
	MuscleMotion classify(std::vector<math::Vector>& values);
};

#endif