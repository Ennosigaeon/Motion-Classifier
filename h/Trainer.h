#ifndef TRAINER_H
#define TRAINER_H

#include <string>
#include <vector>
#include "Classifier.h"
#include "EMGProvider.h"

//TODO: review whole Trainings process
namespace motion_classifier {
	/**
	  * This class handels the task to train a Classifier with
	  * EMG signals before the actual classification process
	  * can be done.
	  */
	class Trainer {
	private:
		static const int recordingTime;

		std::string folder;
		EMGProvider *emgProvider;
		AppConfig *config;
		Status status = Status::NEW;
		std::vector<std::pair<Motion::Muscle, int>> startMotions;

		std::map<Motion::Muscle, std::vector<Interval*>> load();
		void parse();
		void run();

	public:

		/**
		  * Creates a new Trainer. A Trainer can be reused for several
		  * training procedures.
		  */
		Trainer();

		/**
		  * Loads trainings data from the given EMGProvider. By calling
		  * this method a interactive trainings procedure will be started.
		  * It is possible to load old trainings results from the file-
		  * system or create new ones. The caller has the reponsiblity to
		  * delete the returned Intervals.
		  * Throws	std::invalid_argument
		  *			std::underflow_error
		  */
		std::map<Motion::Muscle, std::vector<Interval*>> train(EMGProvider *emgProvider);
	};
}

#endif