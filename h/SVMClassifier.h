#ifndef SVMCLASSIFIER_H
#define SVMCLASSIFIER_H

#include <vector>
#include "AppConfig.h"
#include "ClassifierImpl.h"
#include "MultiClassSVM.h"
#include "Variogram.h"

namespace motion_classifier {

	/**
	  * This class computes the MuscleMotion for EMG signals. The
	  * classification process is done in a own worker Thread. To
	  * communicate with the worker use the member function
	  * send(const Signal&).
	  */
	class SVMClassifier : public ClassifierImpl {
		friend class SVMUtilities;
	private:
		void plot(Sample* sample, std::vector<math::Vector>& values);
		void loadTrainingsData();
		void storeTrainingsData();

		MultiClassSVM *svm;
		Variogram *variogram;
		Properties *prop;

	protected:
		virtual Motion::Muscle classify(Interval *interval);

	public:
		/**
		  * Creates a new SVMClassifier with the given EMGProvider and
		  * Multiclass Support Vector Machine. Default properties are
		  * stored in conf/svm_config.txt.
		  */
		SVMClassifier(EMGProvider *emgProvider, MultiClassSVM *svm, Properties *configuration);

		/**
		  * Stops the underlying worker Thread and the EMGProvider.
		  */
		~SVMClassifier();

		/**
		  * Trains the Classifier.
		  */
		void train(std::map<Motion::Muscle, std::vector<Interval*>*>* values);
	};
}

#endif