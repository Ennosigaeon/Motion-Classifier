
#ifndef MULTI_CLASS_SVM_H
#define MULTI_CLASS_SVM_H

#include <vector>
#include "MuscleMotion.h"
#include "SupportVectorMachine.h"
#include "Vector.h"

class MultiClassSVM {
private:
	std::vector<SupportVectorMachine> svms;
public:
	void train(const MuscleMotion& motion, const std::vector<math::Vector>& values);
	MuscleMotion classify(const std::vector<math::Vector>& values);
};

#endif