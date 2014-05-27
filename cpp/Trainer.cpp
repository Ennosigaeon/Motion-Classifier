
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/AppConfig.h"
#include "../h/Trainer.h"

int Trainer::NR_RUNS = 4;
int Trainer::trainingsSize = 5;

Trainer::Trainer(EMGProvider *emgProvider, MultiClassSVM *svm) {
	Trainer::emgProvider = emgProvider;
	Trainer::svm = svm;
}

void Trainer::train() {
	//Create folder, if it doesn't exists
	std::string BASE_DIR = AppConfig::getInstance()->getTrainerBaseDir();
	boost::filesystem::path root(BASE_DIR);
	if (!boost::filesystem::exists(root))
		boost::filesystem::create_directories(root);

	std::cout << "Please enter username: ";
	std::string user;
	std::cin >> user;
	std::cin.ignore();

	Trainer::folder = BASE_DIR + user + "/";
	boost::filesystem::path folder(Trainer::folder);
	if (boost::filesystem::exists(folder)) {
		std::cout << "Found old data. Load them (y/n)? ";
		std::string answer;
		std::cin >> answer;
		std::cin.ignore();
		if (answer == "y") {
			svm->train(Trainer::folder);
			svm->calculateSVMs();
			return;
		}
		else {
			std::cout << "Do you really want to delete old data from user " << user << " (y/n)?";
			std::cin >> answer;
			std::cin.ignore();
			if (answer == "y")
				boost::filesystem::remove_all(folder);
			else {
				//start trainig procedure again
				train();
				return;
			}
		}
	}
	boost::filesystem::create_directory(Trainer::folder);
	std::cout << "created new user" << std::endl;

	for (int i = 0; i < NR_RUNS; i++) {
		std::cout << "Doing run " << i+1 << " of " << NR_RUNS;

		//rest position
		std::cout << "Hold your arm in the rest position: " << std::endl;
		std::vector<Interval*> rest = collectData(emgProvider);
		store(&rest, Motion::Muscle::REST_POSITION, i);

		//close hand
		std::cout << "Close your hand: " << std::endl;
		std::vector<Interval*> handClose = collectData(emgProvider);
		store(&handClose, Motion::Muscle::HAND_CLOSE, i);

		//open hand
		std::cout << "Open your hand: " << std::endl;
		std::vector<Interval*> handOpen = collectData(emgProvider);
		store(&handOpen, Motion::Muscle::HAND_OPEN, i);

		//wrist flexion
		std::cout << "Roll your hand down in direction of the innerside of the forarm (wrist flextion): " << std::endl;
		std::vector<Interval*> wristFlexion = collectData(emgProvider);
		store(&wristFlexion, Motion::Muscle::WRIST_FLEXION, i);

		//wrist extension
		std::cout << "Pull your hand up in direction of the outerside of the forarm (wrist extension): " << std::endl;
		std::vector<Interval*> wristExtension = collectData(emgProvider);
		store(&wristExtension, Motion::Muscle::WRIST_EXTENSION, i);

		//radial deviation
		std::cout << "Bend your hand outwards, parallel to palm (radial deviation): " << std::endl;
		std::vector<Interval*> radialDeviation = collectData(emgProvider);
		store(&radialDeviation, Motion::Muscle::RADIAL_DEVIATION, i);

		//ulnar deviation
		std::cout << "Bend your hand inwards, parallel to palm (wrist flextion): " << std::endl;
		std::vector<Interval*> ulnarDeviation = collectData(emgProvider);
		store(&ulnarDeviation, Motion::Muscle::ULNAR_DEVIATION, i);

		//forarm pronation
		std::cout << "Rotate your forarm inwards (forarm pronation): " << std::endl;
		std::vector<Interval*> forarmPronation = collectData(emgProvider);
		store(&forarmPronation, Motion::Muscle::FORARM_PRONATION, i);

		//forarm supination
		std::cout << "Rotate your forarm outwards (forarm supination): " << std::endl;
		std::vector<Interval*> forarmSupination = collectData(emgProvider);
		store(&forarmSupination, Motion::Muscle::FORARM_SUPINATION, i);

		svm->train(Trainer::folder);
		svm->calculateSVMs();
	}
}

std::vector<Interval*> Trainer::collectData(EMGProvider *emgProvider) {
	std::vector<Interval*> result;

	std::cout << "Press Enter to start: ";
	std::cin.get();
	emgProvider->send(Signal::START);
	for (int i = 0; i < trainingsSize; i++) {
		Interval *interval = emgProvider->getInterval();
		result.push_back(interval);
	}
	emgProvider->send(Signal::STOP);
	return result;
}

void Trainer::store(std::vector<Interval*> *values, Motion::Muscle motion, int nrRun) {
	std::string file = folder;
	switch (motion) {
	case Motion::Muscle::REST_POSITION:
		file += "REST_POSITION";
		break;
	case Motion::Muscle::FORARM_PRONATION:
		file += "FORARM_PRONATION";
		break;
	case Motion::Muscle::FORARM_SUPINATION:
		file += "FORARM_SUPINATION";
		break;
	case Motion::Muscle::WRIST_FLEXION:
		file += "WRIST_FLEXION";
		break;
	case Motion::Muscle::WRIST_EXTENSION:
		file += "WRIST_EXTENSION";
		break;
	case Motion::Muscle::HAND_OPEN:
		file += "HAND_OPEN";
		break;
	case Motion::Muscle::HAND_CLOSE:
		file += "HAND_CLOSE";
		break;
	case Motion::Muscle::RADIAL_DEVIATION:
		file += "RADIAL_DEVIATION";
		break;
	case Motion::Muscle::ULNAR_DEVIATION:
		file += "ULNAR_DEVIATION";
		break;
	}
	file += "-" + boost::lexical_cast<std::string>(nrRun) + ".txt";
	std::ofstream stream(file);
	for (std::vector<Interval*>::iterator it = values->begin(); it != values->end(); it++)
		stream << (*it)->getRMSSample();
}