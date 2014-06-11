#include <algorithm>
#include <iostream>
#include <map>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread/thread.hpp>
#include "../h/AppConfig.h"
#include "../h/Trainer.h"
#include "../h/Utilities.h"
#include "../h/Variogram.h"

const int Trainer::recordingTime = 3000;

Trainer::Trainer(EMGProvider *emgProvider, MultiClassSVM *svm) {
	Trainer::emgProvider = emgProvider;
	Trainer::svm = svm;
	config = AppConfig::getInstance();
}

void Trainer::train() {
	//Create folder, if it doesn't exists
	std::string BASE_DIR = config->getTrainerBaseDir();
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
			BOOST_LOG_TRIVIAL(info) << "loading trainings data from " << folder;
			load();
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

	//Necessary to avoid problems with deleting a directory and
	//immediatly creating it again.
	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	if (!boost::filesystem::exists(Trainer::folder))
		boost::filesystem::create_directory(Trainer::folder);
	std::cout << "created new user" << std::endl;

	std::cout << "Please press enter to start the training...";
	std::cin.get();
	status = Status::RUNNING;
	std::thread thread = std::thread(&Trainer::run, this);

	std::ofstream out(Trainer::folder + "data.txt");
	emgProvider->send(Signal::START);
	while (status == Status::RUNNING) {
		Interval *interval = emgProvider->getInterval();
		if (interval != NULL)
			for (std::vector<Sample*>::const_iterator it = interval->getSamples().begin(); it != interval->getSamples().end(); ++it)
				out << (**it);
	}
	emgProvider->send(Signal::STOP);
	if (thread.joinable())
		thread.join();

	//Check, that begining of movements are far enough from each other. Otherwise store() could
	//throw a END_OF_FILE exception
	int windowSize = config->getTrainingsSize();
	int previous = -windowSize;
	for (std::vector<std::pair<Motion::Muscle, int>>::iterator it = startMotions.begin(); it != startMotions.end(); ++it) {
		if (abs(it->second - previous) < windowSize) {
			BOOST_LOG_TRIVIAL(fatal) << "The beginnings of the movements are too close to each other. Please restart the training!";
			throw std::underflow_error("beginnings of movements are too close to each other");
		}
		previous = it->second;
	}

	//stores the trainings data
	store();
	//loads the trainings data and trains svm with them
	load();
}

void Trainer::run() {
	int nrRuns = config->getTrainerNrRuns();
	for (int i = 0; i < nrRuns; ++i) {
		std::cout << "Press enter if arm is in rest position: ";
		std::cin.get();
		startMotions.push_back(std::pair<Motion::Muscle, int>(Motion::Muscle::REST_POSITION, emgProvider->getSampleNr()));
		BOOST_LOG_TRIVIAL(debug) << "added " << startMotions.back().second << " as center for " << printMotion(Motion::Muscle::REST_POSITION);
		boost::this_thread::sleep(boost::posix_time::milliseconds(recordingTime));

		/*std::cout << "Press enter before rolling hand downwards (wrist flexion): ";
		std::cin.get();
		startMotions.push_back(std::pair<Motion::Muscle, int>(Motion::Muscle::WRIST_FLEXION, emgProvider->getSampleNr()));
		BOOST_LOG_TRIVIAL(debug) << "added " << startMotions.back().second << " as center for " << printMotion(Motion::Muscle::WRIST_FLEXION);
		boost::this_thread::sleep(boost::posix_time::milliseconds(recordingTime));

		std::cout << "Press enter before rolling hand upwards (wrist extension): ";
		std::cin.get();
		startMotions.push_back(std::pair<Motion::Muscle, int>(Motion::Muscle::WRIST_EXTENSION, emgProvider->getSampleNr()));
		BOOST_LOG_TRIVIAL(debug) << "added " << startMotions.back().second << " as center for " << printMotion(Motion::Muscle::WRIST_EXTENSION);
		boost::this_thread::sleep(boost::posix_time::milliseconds(recordingTime));

		std::cout << "Press enter before bending the hand to the body (radial  deviation): ";
		std::cin.get();
		startMotions.push_back(std::pair<Motion::Muscle, int>(Motion::Muscle::RADIAL_DEVIATION, emgProvider->getSampleNr()));
		BOOST_LOG_TRIVIAL(debug) << "added " << startMotions.back().second << " as center for " << printMotion(Motion::Muscle::RADIAL_DEVIATION);
		boost::this_thread::sleep(boost::posix_time::milliseconds(recordingTime));

		std::cout << "Press enter before bending the hand away from the body (ulnar  deviation): ";
		std::cin.get();
		startMotions.push_back(std::pair<Motion::Muscle, int>(Motion::Muscle::ULNAR_DEVIATION, emgProvider->getSampleNr()));
		BOOST_LOG_TRIVIAL(debug) << "added " << startMotions.back().second << " as center for " << printMotion(Motion::Muscle::ULNAR_DEVIATION);
		boost::this_thread::sleep(boost::posix_time::milliseconds(recordingTime));

		std::cout << "Press enter before rolling the forearm to the body (forearm pronation): ";
		std::cin.get();
		startMotions.push_back(std::pair<Motion::Muscle, int>(Motion::Muscle::FOREARM_PRONATION, emgProvider->getSampleNr()));
		BOOST_LOG_TRIVIAL(debug) << "added " << startMotions.back().second << " as center for " << printMotion(Motion::Muscle::FOREARM_PRONATION);
		boost::this_thread::sleep(boost::posix_time::milliseconds(recordingTime));

		std::cout << "Press enter before rolling the forearm away from the body (forearm supination): ";
		std::cin.get();
		startMotions.push_back(std::pair<Motion::Muscle, int>(Motion::Muscle::FOREARM_SUPINATION, emgProvider->getSampleNr()));
		BOOST_LOG_TRIVIAL(debug) << "added " << startMotions.back().second << " as center for " << printMotion(Motion::Muscle::FOREARM_SUPINATION);
		boost::this_thread::sleep(boost::posix_time::milliseconds(recordingTime));
		
		std::cout << "Press enter before closing the hand (hand close): ";
		std::cin.get();
		startMotions.push_back(std::pair<Motion::Muscle, int>(Motion::Muscle::HAND_CLOSE, emgProvider->getSampleNr()));
		BOOST_LOG_TRIVIAL(debug) << "added " << startMotions.back().second << " as center for " << printMotion(Motion::Muscle::HAND_CLOSE);
		boost::this_thread::sleep(boost::posix_time::milliseconds(recordingTime));

		std::cout << "Press enter before opening the hand (hand open): ";
		std::cin.get();
		startMotions.push_back(std::pair<Motion::Muscle, int>(Motion::Muscle::HAND_OPEN, emgProvider->getSampleNr()));
		BOOST_LOG_TRIVIAL(debug) << "added " << startMotions.back().second << " as center for " << printMotion(Motion::Muscle::HAND_OPEN);
		boost::this_thread::sleep(boost::posix_time::milliseconds(recordingTime));
		*/
	}
	status = Status::FINISHED;
}

void Trainer::store() {
	BOOST_LOG_TRIVIAL(info) << "calculating trainings data from recorded input. This may take some time...";
	int windowSize = config->getTrainingsSize();
	Variogram variogram;

	std::ifstream in(folder + "data.txt");
	if (!in.is_open())
		throw std::invalid_argument("unable to open file");

	//calculate Variograms for every center of movement
	std::map<Motion::Muscle, std::vector<math::Vector>> map;
	int lineNr = 0;
	for (std::vector<std::pair<Motion::Muscle, int>>::const_iterator it = startMotions.begin(); it != startMotions.end(); ++it) {
		if (map.find(it->first) == map.end())
			map.insert(std::pair< Motion::Muscle, std::vector<math::Vector>>(it->first, std::vector<math::Vector>()));
		std::vector<math::Vector> *values = &(map.find(it->first)->second);

		Interval *interval = new Interval();
		while (!in.eof()) {
			if (lineNr < it->second) {
				//TODO: can maybe replaced by ignore
				std::string line;
				std::getline(in, line);
			}
			else {
				if (lineNr <= it->second + windowSize) {
					Sample *s = new Sample();
					in >> *s;
					interval->addSample(s);
					if (interval->isFull()) {
						std::vector<math::Vector> vec = variogram.calculate(interval->getRMSSample());
						values->insert(values->end(), vec.begin(), vec.end());
						delete interval;
						interval = new Interval();
					}
				}
				else
					break;
			}
			++lineNr;
		}
		delete interval;
	}

	//store values in filesystem
	for (std::map<Motion::Muscle, std::vector<math::Vector>>::iterator it = map.begin(); it != map.end(); ++it) {
		std::string s = folder + printMotion(it->first) + ".txt";
		std::ofstream out(s);
		if (!out.is_open())
			throw std::invalid_argument("unable to open file");
		for (std::vector<math::Vector>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			out << *it2 << std::endl;
		out.close();
	}
}

void Trainer::load() {
	if (!boost::filesystem::is_directory(boost::filesystem::path(folder)))
		throw std::invalid_argument("unable to open file");

	int nrRuns = config->getTrainerNrRuns();
	for (int i = Motion::Muscle::REST_POSITION; i <= Motion::Muscle::HAND_CLOSE; ++i) {
		std::string file = folder + printMotion(static_cast<Motion::Muscle>(i)) + ".txt";
		std::ifstream in(file);
		//This trainigs file does not exists
		if (!in.is_open()) {
			BOOST_LOG_TRIVIAL(warning) << "No trainings data for " << printMotion(static_cast<Motion::Muscle>(i)) << " available.";
			continue;
		}

		std::vector<math::Vector> result;
		while (!in.eof()) {
			math::Vector vec;
			in >> vec;
			if (!isnan(vec.getX()))
				result.push_back(vec);
		}
		svm->train(static_cast<Motion::Muscle>(i), result);
		in.close();
	}
	svm->calculateSVMs();

	BOOST_LOG_TRIVIAL(info) << "Training finished. Press Enter to continue...";
	std::cin.get();
}