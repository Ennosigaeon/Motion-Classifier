
#ifndef SPACE_H
#define SPACE_H

#define DIMENSIONS 3

#include <limits>
#include "MathEnums.h"

namespace motion_classifier {

	namespace math {

		/**
		  * Defines a three dimensional vector space. The x and y dimensions are treated as a infinite
		  * pan, i.e. when the vector leaves the plane on the left side it enters it again on the right
		  * side and vice versa. By default the plane has inifite size.
		  */
		class Space {
		private:
			static Norm norm;
			double *min = new double[2];
			double *max = new double[2];

			//Normalizes only first two dimensions, i.e. x and y.
			double* normalize(double *values);
		public:
			Space(double minX = -std::numeric_limits<double>::infinity(), double maxX = std::numeric_limits<double>::infinity(), double minY = -std::numeric_limits<double>::infinity(), double maxY = std::numeric_limits<double>::infinity());
			~Space();

			void setNorm(math::Norm norm);

			double* addition(const double *left, const double *right);
			double* subtraction(const double *left, const double *right);
			double* multiplication(const double *left, double value);
			double* division(const double *left, double value);
			double distance(const double *left, const double *right);
			double length(const double *values, const int nrDimensions = DIMENSIONS);
		};
	}
}

#endif