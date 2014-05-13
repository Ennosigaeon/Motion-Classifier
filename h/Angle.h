
#ifndef ANGLE_H
#define ANGLE_H

#include <iostream>

namespace math {
	enum Angle {
		DEGREE_0, DEGREE_30, DEGREE_60, DEGREE_90, DEGREE_120, DEGREE_150
	};
	//std::ostream& operator<<(std::ostream& stream, const math::Angle& angle) {
	//	switch (angle) {
	//	case DEGREE_0:
	//		stream << "0 degree";
	//		break;
	//	case DEGREE_30:
	//		stream << "30 degree";
	//		break;
	//	case DEGREE_60:
	//		stream << "60 degree";
	//		break;
	//	case DEGREE_90:
	//		stream << "90 degree";
	//		break;
	//	case DEGREE_120:
	//		stream << "120 degree";
	//		break;
	//	case DEGREE_150:
	//		stream << "150 degree";
	//		break;
	//	}
	//	return stream;
	//}
}

#endif