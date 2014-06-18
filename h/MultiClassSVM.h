#ifndef MULTI_CLASS_SVM_H
#define MULTI_CLASS_SVM_H

#include <map>
#include <string>
#include <vector>
#include "SupportVectorMachine.h"
#include "Vector.h"

namespace motion_classifier {
	/**
	  * This class handles the classification of EMG signals.
	  * You have to train the MultiClassSVM with all possible
	  * MuscleMotions. See Trainer for more information about
	  * training. To classify a movement just pass the results
	  * of a Variogram to the classify(const std::vectro<math::Vector>&)
	  * member function.
	  */
	class MultiClassSVM {
	private:
		std::vector<SupportVectorMachine*> svms;
		std::map<Motion::Muscle, std::vector<math::Vector>> trainingsData;

	public:

		/**
		  * Destroys the MultiClassSVM and all SupportVectorMachines.
		  */
		~MultiClassSVM();

		/**
		  * Trains the MultiClassSVM with the given Variogram result for
		  * the given Motion::Muscle. It is recommended, that you pass
		  * as much values as posible, because this will increase the
		  * accuarcy.
		  */
		void train(const Motion::Muscle& motion, const std::vector<math::Vector>& values);

		/**
		  * Creates all Support Vector Machines from the trainings data.
		  * You have to call this function, otherwise no classifcation will be
		  * possible. Once you have called this function it is not possible
		  * to add anymore trainings data.
		  */
		void calculateSVMs();

		/**
		  * Classifies the given Variogram result and returns the result as
		  * a Motion::Muscle. When the movement can not be classified, the
		  * Motion::Muscle UNKNOWN will be returned.
		  */
		Motion::Muscle classify(std::vector<math::Vector>& values);
	};
}

#endif