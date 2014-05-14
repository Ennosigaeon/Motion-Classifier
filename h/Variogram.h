
#ifndef VARIOGRAM_H
#define VARIOGRAM_H

#include <map>
#include <vector>
#include "Sample.h"
#include "Vector.h"

/**
  * This class calculates a Variogram for a given Sample.
  */
class Variogram {
private:
	std::map<double, double> findPairs(std::vector<math::Vector>& values, const math::Vector& h, const int maxDist) const;
	int nrBins;

public:
	Variogram();

	/**
	  * Calculates the Variogram value for the given Sample. The Variogram is
	  * calculated for every math:Angle. The calculated values are stored as
	  * a two dimensional vector (the actual gap, consisting out of angle and
	  * length) and the measured value at this position.
	  */
	std::vector<math::Vector> calculate(const Sample& sample) const;
};

#endif VARIOGRAM_H