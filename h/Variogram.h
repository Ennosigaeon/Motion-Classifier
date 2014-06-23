#ifndef VARIOGRAM_H
#define VARIOGRAM_H

#include <vector>
#include "Sample.h"
#include "Vector.h"

namespace motion_classifier {

	/**
	  * This class calculates a Variogram for a given Sample.
	  */
	class Variogram {
	private:
		double calc(Sample* sample, const math::Vector& h, const double precision, int *count) const;
		int nrBins;

	public:
		Variogram(int nrBins);

		/**
		  * Calculates the Variogram values for the given Sample. The Variogram is
		  * calculated for every math:Angle. The calculated values are stored as
		  * a two dimensional vector (the actual gap, consisting out of angle and
		  * length) and the measured value at this position.
		  */
		std::vector<math::Vector> calculate(Sample* sample) const;
	};
}

#endif VARIOGRAM_H