#ifndef TRAINER_H
#define TRAINER_H

#include <string>
#include <vector>
#include "EMGProvider.h"
#include "MultiClassSVM.h"

namespace motion_classifier {
	/**
	  * This class handels the task to train a MultiClassSVM
	  * with EMG signals before the actual classification process
	  * can be done.
	  */
	class Trainer {
	private:
		static const int recordingTime;

		std::string folder;
		AppConfig *config;
		EMGProvider *emgProvider;
		MultiClassSVM *svm;
		Status status = Status::NEW;
		std::vector<std::pair<Motion::Muscle, int>> startMotions;

		void load();
		void store();
		void run();

	public:

		/**
		  * Creates a new Trainer. The Trainer expects a pointer to an
		  * EMGProvider and a pointer to a MultiClassSVM. By calling train()
		  * the training procedure will be started and the given Multi-
		  * ClassSVM will be trained with EMG values from the EMGProvider.
		  */
		Trainer(EMGProvider *emgProvider, MultiClassSVM *svm);

		/**
		  * By calling this method a interactive trainings procedure will
		  * be started. It is possible to load old trainings results from
		  * the filesystem or create new ones.
		  * Throws	std::invalid_argument
		  *			std::underflow_error
		  */
		void train();
	};
}

#endif