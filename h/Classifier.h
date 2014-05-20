#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include "AppConfig.h"
#include "BlockingQueue.h"
#include "Communication.h"
#include "EMGProvider.h"
#include "MultiClassSVM.h"
#include "MuscleMotion.h"
#include "Variogram.h"

/**
  * This class computes the MuscleMotion for EMG signals. The
  * classification process is done in an own worker Thread. To
  * communicate with the worker use the member function
  * send(const Signal&).
  */
class Classifier {
private:
	std::thread worker;
	std::mutex mutex;
	std::condition_variable condition;
	void run();
	void plot(const Sample& sample, std::vector<math::Vector>& values);

	BlockingQueue<MuscleMotion*> lastMuscleMotion{ 1, AppConfig::getInstance()->getBlockingQueueMaxWaitTime() };
	MultiClassSVM svm;
	Variogram variogram;
	EMGProvider *emgProvider;
	AppConfig *config;
	Status status = NEW;
	int nr = 0;

public:

	/**
	  * Creates a new Classifier with the given EMGProvider.
	  */
	Classifier(EMGProvider *emgProvider);

	/**
	  * Stops the underlying worker Thread and the EMGProvider.
	  */
	~Classifier();

	/**
	  * Returns the last stored MuscleMotion. The stored value is
	  * deleted by this operation. When no MuscleMotion is
	  * available, the calling Thread will be blocked until an new
	  * MuscleMotion is available.
	  */
	MuscleMotion getMuscleMotion();

	/**
	  * Sends the given signal to the underlying worker Thread.
	  */
	void send(const Signal& signal);

	/**
	  * Trains the Classifier with the given data for the given motion.
	  * You have to provide data for every MuscleMotion. Otherwise the
	  * classification is not reliable.
	  */
	void train(const MuscleMotion& motion, std::vector<math::Vector>& data);
};

#endif