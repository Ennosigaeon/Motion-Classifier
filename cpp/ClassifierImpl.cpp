#include <boost/lexical_cast.hpp>
#include "../h/ClassifierImpl.h"


using namespace motion_classifier;

Motion::Muscle ClassifierImpl::getMuscleMotion() {
	Motion::Muscle *motion = lastMuscleMotion.pop();
	if (motion == NULL)
		return Motion::Muscle::UNKNOWN;
	else
		return *motion;
}

std::string ClassifierImpl::printStatistics() {
	std::string s("Classified " + boost::lexical_cast<std::string>(intervalCount)+" Samples in avg. " + boost::lexical_cast<std::string>(time / intervalCount) + " ms");
	return s;
}