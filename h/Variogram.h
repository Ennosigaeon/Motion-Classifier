
#ifndef VARIOGRAM_H
#define VARIOGRAM_H

#include <map>
#include <vector>
#include "Sample.h"
#include "Vector.h"

class Variogram {
private:
	std::map<double, double> findPairs(std::vector<math::Vector>& values, const math::Vector& h, const int maxDist) const;
	int nrBins;

public:
	Variogram();

	//returns all positions with the corresponding values for the given Sample and all math::Angles.
	std::vector<math::Vector> calculate(const Sample& sample) const;
};

#endif VARIOGRAM_H