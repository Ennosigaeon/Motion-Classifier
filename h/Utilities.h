#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include "MathEnums.h"
#include "Motion.h"

namespace motion_classifier {
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
}

#endif