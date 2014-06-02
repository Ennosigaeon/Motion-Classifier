#ifndef TRAINER_H
#define TRAINER_H

#include <string>
#include <vector>
#include "EMGProvider.h"
#include "MultiClassSVM.h"

class Trainer {
private:
	std::string folder;
	EMGProvider *emgProvider;
	MultiClassSVM *svm;

	std::vector<Interval*> collectData();
	void store(std::vector<Interval*>* values, Motion::Muscle& motion, int runNr);
	void load();

public:
	Trainer(EMGProvider *emgProvider, MultiClassSVM *svm);

	void train();
};

#endif