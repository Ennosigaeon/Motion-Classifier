#ifndef MATHENUMS_H
#define MATHENUMS_H

namespace motion_classifier {

	namespace math {

		/**
		  * This enum is used to specifiy all angles that are relevant for the calculation
		  * of the variogram.
		  */
		enum Angle {
			DEGREE_0,
			DEGREE_30,
			DEGREE_60,
			DEGREE_90,
			DEGREE_120,
			DEGREE_150
		};

		enum KernelType {
			UNIFORM,
			GAUSSIAN,
			EPANECHNIKOV
		};

		enum Norm {
			L1,
			L2
		};

		enum Dimension {
			X,
			Y,
			Z
		};
	}
}


#endif