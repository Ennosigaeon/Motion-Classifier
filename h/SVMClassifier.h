#ifndef SVMCLASSIFIER_H
#define SVMCLASSIFIER_H

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include "AppConfig.h"
#include "BlockingQueue.h"
#include "Classifier.h"
#include "Communication.h"
#include "EMGProvider.h"
#include "MultiClassSVM.h"
#include "Variogram.h"

namespace motion_classifier {

	/**
	  * This class computes the MuscleMotion for EMG signals. The
	  * classification process is done in a own worker Thread. To
	  * communicate with the worker use the member function
	  * send(const Signal&).
	  */
	class SVMClassifier : public Classifier {
	private:
		std::thread worker;
		std::mutex mutex;
		std::condition_variable condition;
		void run();
		void plot(Sample* sample, std::vector<math::Vector>& values);
		void loadTrainingsData();
		void storeTrainingsData();

		MultiClassSVM *svm;
		Variogram variogram;
		EMGProvider *emgProvider;

	public:
		/**
		  * Creates a new SVMClassifier with the given EMGProvider and
		  * Multiclass Support Vector Machine.
		  */
		SVMClassifier(EMGProvider *emgProvider, MultiClassSVM *svm);

		/**
		  * Stops the underlying worker Thread and the EMGProvider.
		  */
		~SVMClassifier();

		virtual Motion::Muscle getMuscleMotion();

		virtual void send(const Signal& signal);

		virtual std::string printStatistics();

		/**
		  * Trains the Classifier.
		  */
		void train(const std::map<Motion::Muscle, std::vector<Interval*>>& values);
	};
}

#endif