#ifndef MSCLASSIFIER_H
#define MSCLASSIFIER_H

#include "ClassifierImpl.h"
#include "Kernel.h"
#include "MeanShift.h"
#include "Properties.h"

namespace motion_classifier {

	class MSClassifier : public ClassifierImpl {
	private:
		MeanShift *msAlgo;
		//Bandwidth for MeanShift
		double h;

		//Number buckets for classification matrix
		double m;
		double n;
		double p;

		std::map<Motion::Muscle, std::vector<math::Vector*>*> trainingsData;

		std::map< Motion::Muscle, std::vector<Interval*>*>* extractTrainingsData(std::string folder);

	protected:
		virtual Motion::Muscle classify(Interval *interval);

	public:

		/**
		  * Creates an new Classifier based on the Mean Shift algorithm.
		  */
		MSClassifier(EMGProvider *emg, Properties *configuration);

		~MSClassifier();

		void train(std::string folder);

		//TODO: remove again
		std::map<Motion::Muscle, std::vector<math::Vector*>*>* getTrainingsData();
	};

}

#endif