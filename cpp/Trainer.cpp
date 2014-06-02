#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/AppConfig.h"
#include "../h/Exception.h"
#include "../h/Trainer.h"
#include "../h/Utilities.h"m

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
			load();
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

	//TODO: this code is wrong
	//for (int i = 0; i < NR_RUNS; i++) {
	//	std::cout << "Doing run " << i+1 << " of " << NR_RUNS;

	//	//rest position
	//	std::cout << "Hold your arm in the rest position: " << std::endl;
	//	std::vector<Interval*> rest = collectData();
	//	store(&rest, Motion::Muscle::REST_POSITION, i);

	//	//close hand
	//	std::cout << "Close your hand: " << std::endl;
	//	std::vector<Interval*> handClose = collectData();
	//	store(&handClose, Motion::Muscle::HAND_CLOSE, i);

	//	//open hand
	//	std::cout << "Open your hand: " << std::endl;
	//	std::vector<Interval*> handOpen = collectData();
	//	store(&handOpen, Motion::Muscle::HAND_OPEN, i);

	//	//wrist flexion
	//	std::cout << "Roll your hand down in direction of the innerside of the forarm (wrist flextion): " << std::endl;
	//	std::vector<Interval*> wristFlexion = collectData();
	//	store(&wristFlexion, Motion::Muscle::WRIST_FLEXION, i);

	//	//wrist extension
	//	std::cout << "Pull your hand up in direction of the outerside of the forarm (wrist extension): " << std::endl;
	//	std::vector<Interval*> wristExtension = collectData();
	//	store(&wristExtension, Motion::Muscle::WRIST_EXTENSION, i);

	//	//radial deviation
	//	std::cout << "Bend your hand outwards, parallel to palm (radial deviation): " << std::endl;
	//	std::vector<Interval*> radialDeviation = collectData();
	//	store(&radialDeviation, Motion::Muscle::RADIAL_DEVIATION, i);

	//	//ulnar deviation
	//	std::cout << "Bend your hand inwards, parallel to palm (wrist flextion): " << std::endl;
	//	std::vector<Interval*> ulnarDeviation = collectData();
	//	store(&ulnarDeviation, Motion::Muscle::ULNAR_DEVIATION, i);

	//	//forarm pronation
	//	std::cout << "Rotate your forarm inwards (forarm pronation): " << std::endl;
	//	std::vector<Interval*> forarmPronation = collectData();
	//	store(&forarmPronation, Motion::Muscle::FORARM_PRONATION, i);

	//	//forarm supination
	//	std::cout << "Rotate your forarm outwards (forarm supination): " << std::endl;
	//	std::vector<Interval*> forarmSupination = collectData();
	//	store(&forarmSupination, Motion::Muscle::FORARM_SUPINATION, i);

	//	svm->train(Trainer::folder);
	//	svm->calculateSVMs();
	//}
}

//TODO: this function is wrong
std::vector<Interval*> Trainer::collectData() {
	std::vector<Interval*> result;

	std::cout << "Press Enter to start: ";
	std::cin.get();
	emgProvider->send(Signal::START);
	for (int i = 0; i < 0; i++) {
		Interval *interval = emgProvider->getInterval();
		result.push_back(interval);
	}
	emgProvider->send(Signal::STOP);
	return result;
}

void Trainer::store(std::vector<Interval*> *values, Motion::Muscle motion, int nrRun) {
	std::string file = folder + printMotion(motion) + "-" + boost::lexical_cast<std::string>(nrRun)+".txt";
	std::ofstream stream(file);
	for (std::vector<Interval*>::iterator it = values->begin(); it != values->end(); it++)
		stream << (*it)->getRMSSample();
}

void Trainer::load() {
	if (!boost::filesystem::is_directory(boost::filesystem::path(folder)))
		throw Exception::UNABLE_TO_OPEN_FILE;

	AppConfig *config = AppConfig::getInstance();
	BOOST_LOG_TRIVIAL(info) << "loading trainings data from " << folder;
	for (int i = Motion::Muscle::REST_POSITION; i <= Motion::Muscle::HAND_CLOSE; i++) {
		std::string file = folder + printMotion(static_cast<Motion::Muscle>(i)) + "-";

		std::vector<math::Vector> result;
		for (int j = 0; j < config->getTrainerNrRuns(); j++) {
			std::string tmp = file;
			tmp += boost::lexical_cast<std::string>(j)+".txt";
			std::ifstream in(tmp);
			//This trainigs file does not exists
			if (!in.is_open())
				continue;
			while (!in.eof()) {
				math::Vector vec;
				in >> vec;
				result.push_back(vec);
			}
		}
		svm->train(static_cast<Motion::Muscle>(i), result);
	}
}