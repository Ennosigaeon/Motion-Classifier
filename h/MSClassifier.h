#ifndef MSCLASSIFIER_H
#define MSCLASSIFIER_H

#define NR_INTERVALS 1

#include "ClassifierImpl.h"
#include "Kernel.h"
#include "Matrix.h"
#include "MeanShift.h"
#include "Properties.h"

namespace motion_classifier {

	class MSClassifier : public ClassifierImpl {
		friend CrossCorrelation;
	private:
		MeanShift *msAlgo;
		//Bandwidth for MeanShift
		double h;

		//Number buckets for classification matrix
		int m;
		int n;
		int p;

		std::map<Motion::Muscle, math::Matrix*> trainingsData;
		std::map<Motion::Muscle, std::vector<math::Vector*>*> trainingsVectors;

		std::map< Motion::Muscle, std::vector<Interval*>*>* extractTrainingsData(std::string folder);
		void calcTrainingsMatrix();

	protected:
		virtual Motion::Muscle classify(Interval *interval);

	public:

		/**
		  * Creates an new Classifier based on the Mean Shift algorithm.
		  */
		MSClassifier(EMGProvider *emg, Properties *configuration);

		~MSClassifier();

		void train(std::string folder);
	};

}

#endif