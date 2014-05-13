
#ifndef ANGLE_H
#define ANGLE_H

namespace math {

	/**
	  * This enum is used to specifiy all angles that are relevant for the calculation
	  * of the variogram.
	  */
	enum Angle {
		DEGREE_0, DEGREE_30, DEGREE_60, DEGREE_90, DEGREE_120, DEGREE_150
	};

	//TODO: It would be nicer, to be able to print out this enum.
}

#endif