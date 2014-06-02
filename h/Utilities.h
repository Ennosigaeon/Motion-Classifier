#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <vector>
#include "Angle.h"
#include "Motion.h"

/**
  * Converts the given Motion::Muscle in a std::string.
  * Use getMotion() to retrieve the Motion::Muscle again.
  */
std::string printMotion(const Motion::Muscle& motion);

/**
  * Converts the given std::string in a Motion::Muscle.
  * Use printMotion() to retrieve the according std::string.
  */
Motion::Muscle getMotion(const std::string& string);

/**
  * Converts the given math::Angle in a std::string.
  */
std::string printAngle(const math::Angle& angle);

/**
  * Converts a file with EMG signals into to the format that is expected by
  * the MotionClassifier. The input file has to have the format
  *     <VALUE> <WHITESPACES> <VALUE> <WHITESPACES> <VALUE>...
  * Linebreaks are ignored. The output format is
  *     <VALUE> <BLANK> <VALUE> <BLANK> <VALUE> <BLANK> <VALUE> <BLANK>...
  * With nrColumns * nrRows (defined in configuration file) values in a
  * single line. So each line equals one Sample.
  */
void convertFile(const std::string& inputFile, const std::string& outputFile);

/**
  * Extracts the trainings data from the given file and stores them under the
  * given user name. The input file has to be formated by convertFile(). The
  * results can be used as input for the Trainer.
  * values is a collection of Motion::Muscle and the according center of the
  * movement. The center of the movement is the number of the Sample that lies
  * in the middle of a Movement.
  */
void createTrainingsData(const std::string& inputFile, const std::string& userName, std::vector<std::pair<Motion::Muscle, int>>& values);

#endif