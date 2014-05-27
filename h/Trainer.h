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

	std::vector<Interval*> collectData(EMGProvider *emgProvider);
	void store(std::vector<Interval*>* values, Motion::Muscle motion, int runNr);

public:
	static int NR_RUNS;
	static int trainingsSize;
	Trainer(EMGProvider *emgProvider, MultiClassSVM *svm);

	void train();
};

#endif