#ifndef SUPPORT_VECTOR_MACHINE_H
#define SUPPORT_VECTOR_MACHINE_H

#include <vector>
#include "../lib/libsvm-3.18/svm.h"
#include "AppConfig.h"
#include "Motion.h"
#include "Vector.h"

/**
  * This class is a wrapper for the libsvm Support Vector Machine.
  */
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

	/**
	  * Adds the collection of math::Vectors for the given Motion::Muscle to
	  * the libsvm Support Vector Machine. The values are converted into the
	  * correct data format.
	  */
	void addTrainData(const Motion::Muscle& motion, std::vector<math::Vector>& values);

	/**
	  * Calculates the Support Vector Machine from the given trainings data.
	  * You have to provide trainings data for both classes before calling this
	  * member function.
	  * Throws	std::domain_error
	  */
	void calculateSVM();

	/**
	  * Assigns the given collection of variogram values to one of
	  * the both trained classes.
	  */
	Motion::Muscle classify(std::vector<math::Vector>& values);
};

#endif