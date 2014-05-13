
#ifndef MULTI_CLASS_SVM_H
#define MULTI_CLASS_SVM_H

#include <map>
#include <vector>
#include "MuscleMovement.h"
#include "SupportVectorMachine.h"
#include "Vector.h"

class MultiClassSVM {
private:
	std::vector<SupportVectorMachine> svms;
public:
	void train(const MuscleMovement& muscleMovement, const std::map<math::Vector, double> values);
	MuscleMovement classify(const std::map<math::Vector, double> values);
};

#endif