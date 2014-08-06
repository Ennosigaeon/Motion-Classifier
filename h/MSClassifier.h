
#ifndef MSCLASSIFIER_H
#define MSCLASSIFIER_H

#include <thread>

#include "ClassifierImpl.h"
#include "EMGProvider.h"
#include "Kernel.h"
#include "MeanShift.h"
#include "Properties.h"

namespace motion_classifier {

	class MSClassifier : public ClassifierImpl {
	private:
		Logger *logger;
		MeanShift *msAlgo;
		double h;
		std::map<Motion::Muscle, std::vector<math::Vector*>*> trainingsData;

		EMGProvider *emgProvider;
		std::thread worker;
		std::mutex mutex;
		std::condition_variable condition;

		void run();
	public:

		/**
		  * Creates an new Classifier based on the Mean Shift algorithm.
		  */
		MSClassifier(EMGProvider *emg, Properties *configuration);

		~MSClassifier();

		virtual void send(const Signal& signal);

		void train(std::map<Motion::Muscle, std::vector<Interval*>*>* traingsData);

		//TODO: remove again
		std::map<Motion::Muscle, std::vector<math::Vector*>*>* getTrainingsData();
		std::map< Motion::Muscle, std::vector<Interval*>*>* extractTrainingsData(std::string folder);
	};

}

#endif