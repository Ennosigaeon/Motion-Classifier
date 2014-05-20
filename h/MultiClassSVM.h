#ifndef MULTI_CLASS_SVM_H
#define MULTI_CLASS_SVM_H

#include <vector>
#include "MuscleMotion.h"
#include "SupportVectorMachine.h"
#include "Vector.h"

/**
  * This class handles the classification of EMG signals.
  * You have to train the MultiClassSVM with all possible
  * MuscleMotions. To classify a movement just pass a the
  * of a Variogram to the classify(const std::vectro<math::Vector>&)
  * member function.
  */
class MultiClassSVM {
private:
	std::vector<SupportVectorMachine> svms;
public:

	/**
	  * Train the MultiClassSVM with the given Variogram result for the
	  * given MuscleMotion. It is recommended, that you pass multiple
	  * variogram results for every MuscleMotion, this will improve
	  * the accuracy of the results.
	  */
	void train(const MuscleMotion& motion, const std::vector<math::Vector>& values);

	/**
	  * Classifies the given Variogram result and returns the result as
	  * a MuscleMotion. When the movement can not be classified, the
	  * MuscleMotion UNKNOWN will be returned.
	  */
	MuscleMotion classify(const std::vector<math::Vector>& values);
};

#endif