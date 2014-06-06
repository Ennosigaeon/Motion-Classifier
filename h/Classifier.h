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
#include "Variogram.h"

/**
  * This class computes the MuscleMotion for EMG signals. The
  * classification process is done in a own worker Thread. To
  * communicate with the worker use the member function
  * send(const Signal&).
  */
class Classifier {
private:
	std::thread worker;
	std::mutex mutex;
	std::condition_variable condition;
	void run();
	void plot(Sample* sample, std::vector<math::Vector>& values);

	BlockingQueue<Motion::Muscle*> lastMuscleMotion{ 1, AppConfig::getInstance()->getBlockingQueueMaxWaitTime() };
	MultiClassSVM *svm;
	Variogram variogram;
	EMGProvider *emgProvider;
	AppConfig *config;
	Status status = NEW;
	int intervalCount = 0;
	long time = 0;

public:

	/**
	  * Creates a new Classifier with the given EMGProvider and
	  * Multiclass Support Vector Machine.
	  */
	Classifier(EMGProvider *emgProvider, MultiClassSVM *svm);

	/**
	  * Stops the underlying worker Thread and the EMGProvider.
	  */
	~Classifier();

	/**
	  * Returns the last stored MuscleMotion. The stored value is
	  * deleted by this operation. When no MuscleMotion is
	  * available, the calling Thread will be blocked until an new
	  * MuscleMotion is available. When a maximum waiting time is
	  * defined and the timeout is reached UNKNOWN will be returned.
	  */
	Motion::Muscle getMuscleMotion();

	/**
	  * Sends the given signal to the underlying worker Thread.
	  */
	void send(const Signal& signal);
};

#endif