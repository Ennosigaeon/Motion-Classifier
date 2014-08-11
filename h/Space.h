
#ifndef SPACE_H
#define SPACE_H

#define DIMENSIONS 3

#include <limits>
#include "MathEnums.h"

namespace motion_classifier {
	namespace math {

		/**
		  * Defines a (limited) Space for vector operations.
		  **/
		class Space {
		private:
			static Norm norm;
			double min[DIMENSIONS];
			double max[DIMENSIONS];

		public:
			Space(double minX = -std::numeric_limits<double>::infinity(), double maxX = std::numeric_limits<double>::infinity(),
				double minY = -std::numeric_limits<double>::infinity(), double maxY = std::numeric_limits<double>::infinity(),
				double minZ = -std::numeric_limits<double>::infinity(), double maxZ = std::numeric_limits<double>::infinity());

			double getMin(math::Dimension dimension) const;

			double getMax(math::Dimension dimension) const;
		};
	}
}

#endif