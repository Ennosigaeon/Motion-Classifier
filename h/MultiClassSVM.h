#ifndef MULTI_CLASS_SVM_H
#define MULTI_CLASS_SVM_H

#include <map>
#include <string>
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
	std::vector<SupportVectorMachine*> svms;
	std::map<MuscleMotion, std::vector<math::Vector>> trainingsData;

public:
	~MultiClassSVM();

	/**
	  * Train the MultiClassSVM with the given Variogram result for the
	  * given MuscleMotion. It is recommended, that you pass multiple
	  * variogram results for every MuscleMotion, this will improve
	  * the accuracy of the results.
	  */
	void train(const MuscleMotion& motion, const std::vector<math::Vector>& values);


	/**
	  * Creates all Support Vector Machines from the trainings data.
	  * You have to call this function, otherwise no classifcation will be
	  * possible. Once you have called this function it is not possible
	  * to add anymore trainings data.
	  */
	void calculateSVMs();

	/**
	  * Classifies the given Variogram result and returns the result as
	  * a MuscleMotion. When the movement can not be classified, the
	  * MuscleMotion UNKNOWN will be returned.
	  */
	MuscleMotion classify(std::vector<math::Vector>& values);

	/**
	  * Stores the calculated Multiclass Support Vector Machine in the
	  * file system. So you don't have to train the Multiclass SVM in
	  * every single run.
	  * Throws: Exception::USER_NAME_NOT_AVAILABLE
	  */
	void store(std::string userName);

	/**
	  * Loads a precalculated Multiclass Support Vector Machine from
	  * the file system. You don't have to train the Multiclass SVM
	  * again with new values.
	  * Throws: Exception::USERNAME_NOT_FOUND
	  */
	void train(std::string userName);
};

#endif