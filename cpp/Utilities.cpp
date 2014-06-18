#include <fstream>
#include <sstream>
#include "../h/AppConfig.h"
#include "../h/Utilities.h"

using namespace motion_classifier;

std::string motion_classifier::printMotion(const Motion::Muscle& motion) {
	switch (motion) {
	case Motion::Muscle::REST_POSITION:
		return "REST_POSITION";
	case Motion::Muscle::FOREARM_PRONATION:
		return "FOREARM_PRONATION";
	case Motion::Muscle::FOREARM_SUPINATION:
		return "FOREARM_SUPINATION";
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
	if (string == "FOREARM_PRONATION")
		return Motion::Muscle::FOREARM_PRONATION;
	if (string == "FOREARM_SUPINATION")
		return Motion::Muscle::FOREARM_SUPINATION;
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

std::string motion_classifier::printAngle(const math::Angle& angle) {
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
		throw std::invalid_argument("unable to open at least one file");

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
