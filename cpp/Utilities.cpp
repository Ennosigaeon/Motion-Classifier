#include <algorithm>
#include <fstream>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "../h/AppConfig.h"
#include "../h/Exception.h"
#include "../h/Interval.h"
#include "../h/Utilities.h"
#include "../h/Variogram.h"

#include <iostream>

std::string printMotion(const Motion::Muscle& motion) {
	switch (motion) {
	case Motion::Muscle::REST_POSITION:
		return "REST_POSITION";
	case Motion::Muscle::FORARM_PRONATION:
		return "FORARM_PRONATION";
	case Motion::Muscle::FORARM_SUPINATION:
		return "FORARM_SUPINATION";
	case Motion::Muscle::WRIST_FLEXION:
		return "WRIST_FLEXION";
	case Motion::Muscle::WRIST_EXTENSION:
		return "WRIST_EXTENSION";
	case Motion::Muscle::HAND_OPEN:
		return "HAND_OPEN";
	case Motion::Muscle::HAND_CLOSE:
		return "HAND_CLOSE";
	case Motion::Muscle::RADIAL_DEVIATION:
		return "RADIAL_DEVIATION";
	case Motion::Muscle::ULNAR_DEVIATION:
		return "ULNAR_DEVIATION";
	default:
		return "UNKNOWN";
	}
}

Motion::Muscle getMotion(const std::string& string) {
	if (string == "REST_POSITION")
		return Motion::Muscle::REST_POSITION;
	if (string == "FORARM_PRONATION")
		return Motion::Muscle::FORARM_PRONATION;
	if (string == "FORARM_SUPINATION")
		return Motion::Muscle::FORARM_SUPINATION;
	if (string == "WRIST_FLEXION")
		return Motion::Muscle::WRIST_FLEXION;
	if (string == "WRIST_EXTENSION")
		return Motion::Muscle::WRIST_EXTENSION;
	if (string == "HAND_OPEN")
		return Motion::Muscle::HAND_OPEN;
	if (string == "HAND_CLOSE")
		return Motion::Muscle::HAND_CLOSE;
	if (string == "RADIAL_DEVIATION")
		return Motion::Muscle::RADIAL_DEVIATION;
	if (string == "ULNAR_DEVIATION")
		return Motion::Muscle::ULNAR_DEVIATION;
	return Motion::Muscle::UNKNOWN;
}

std::string printAngle(const math::Angle& angle) {
	switch (angle) {
	case math::Angle::DEGREE_0:
		return "0 degree";
	case math::Angle::DEGREE_30:
		return "30 degree";
	case math::Angle::DEGREE_60:
		return "60 degree";
	case math::Angle::DEGREE_90:
		return "90 degree";
	case math::Angle::DEGREE_120:
		return "120 degree";
	case math::Angle::DEGREE_150:
		return "150 degree";
	default:
		return "unknown angle";
	}
}

void convertFile(const std::string& inputFile, const std::string& outputFile) {
	AppConfig *config = AppConfig::getInstance();
	int sampleSize = config->getSampleColumns() * config->getSampleRows();

	int count = 0;
	std::string line;
	std::ifstream in(inputFile, std::ifstream::in);
	std::ofstream out(outputFile);
	if (!in.is_open() || !out.is_open())
		throw Exception::UNABLE_TO_OPEN_FILE;

	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream splitter(line);
		while (splitter) {
			std::string sub;
			splitter >> sub;
			if (!sub.empty()) {
				out << sub << " ";
				++count;
			}
			if (count != 0 && count % sampleSize == 0) {
				out << std::endl;
				count = 0;
			}
		}
	}
	in.close();
	out.close();
}

void createTrainingsData(const std::string& inputFile, const std::string& username, std::vector<std::pair<Motion::Muscle, int>>& values) {
	AppConfig *config = AppConfig::getInstance();
	int windowSize = config->getTrainingsSize();
	std::string folder = config->getTrainerBaseDir() + username + "/";

	boost::filesystem::path root(folder);
	if (!boost::filesystem::exists(root))
		boost::filesystem::create_directories(root);


	Variogram variogram;

	std::ifstream in(inputFile);
	if (!in.is_open())
		throw Exception::UNABLE_TO_OPEN_FILE;

	//sort vector by center of movement ascending
	std::sort(values.begin(), values.end(),
		boost::bind(&std::pair<Motion::Muscle, int>::second, _1) <
		boost::bind(&std::pair<Motion::Muscle, int>::second, _2));

	int count[Motion::Muscle::HAND_CLOSE + 1] = {};
	int lineNr = 0;
	for (std::vector<std::pair<Motion::Muscle, int>>::const_iterator it = values.begin(); it != values.end(); ++it) {
		std::string s = folder + printMotion(it->first) + "-" + boost::lexical_cast<std::string>(count[it->first]) + ".txt";
		std::ofstream out(s);
		if (!out.is_open())
			throw Exception::UNABLE_TO_OPEN_FILE;
		++count[it->first];

		int nr = it->second - windowSize / 2;
		bool found = false;
		
		Interval *interval = new Interval();
		while (!in.eof()) {
			++lineNr;
			if (lineNr == nr) {
				found = true;
				std::cout << "found data for " << printMotion(it->first) << " at " << lineNr << std::endl;
			}
			if (lineNr >= nr && lineNr <= nr + windowSize) {
				Sample *s = new Sample(config->getSampleRows(), config->getSampleColumns());
				in >> *s;
				interval->addSample(s);
				if (interval->isFull()) {
					std::vector<math::Vector> vec = variogram.calculate(interval->getRMSSample());
					for (std::vector<math::Vector>::iterator it = vec.begin(); it != vec.end(); ++it)
						out << *it << std::endl;
					delete interval;
					interval = new Interval();
				}
			}
			else {
				std::string line;
				std::getline(in, line);
				if (found)
					break;
			}
		}
		out.close();
		delete interval;
	}
}