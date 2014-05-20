
//#include "../h/Plotter.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "../h/AppConfig.h"
#include "../h/Variogram.h"

Variogram::Variogram() {
	nrBins = AppConfig::getInstance()->getVariogramNrBins();
}

std::vector<math::Vector> Variogram::calculate(const Sample& sample) const {
	std::vector<math::Vector> result;

	int maxX = sample.getNrColumns() / 2;
	int maxY = sample.getNrRows() / 2;
	clock_t t = clock();
	for (int i = math::Angle::DEGREE_0; i <= math::Angle::DEGREE_150; i++) {
		math::Angle angle = static_cast<math::Angle>(i);
		math::Vector h = math::Vector::getVector(angle);

		BOOST_LOG_TRIVIAL(debug) << "using Angle " << angle;
		while (std::abs(h.get(0)) <= maxX && std::abs(h.get(1)) <= maxY) {
			std::map<double, double> pairs = findPairs(sample.getEntries(), h, std::max(maxX, maxY));

			BOOST_LOG_TRIVIAL(debug) << "found " << pairs.size() << " pairs for the offset " << h;
			if (pairs.size() != 0) {
				double value = 0;
				for (std::map<double, double>::iterator it = pairs.begin(); it != pairs.end(); it++)
					value += pow(it->first - it->second, 2);
				math::Vector tmp = h;
				tmp.setGroup(angle);
				tmp.setZ(value / (2 * pairs.size()));
				result.push_back(tmp);
			}
			h.setLength(h.getLength() + 1);
		}
	}
	t = clock() - t;
	BOOST_LOG_TRIVIAL(debug) << "Variogram calculation took " << ((double)t) / CLOCKS_PER_SEC * 1000 << " ms";

	return result;
}

std::map<double, double> Variogram::findPairs(std::vector<math::Vector>& values, const math::Vector& h, const int maxDist) const {
	std::map<double, double> result;
	double precision = maxDist * 1.0 / nrBins;

	for (std::vector<math::Vector>::iterator it = values.begin(); it != values.end(); it++) {
		if (it->getGroup() != -1)
			continue;
		math::Vector point(it->getX() + h.getX(), it->getY() + h.getY(), 0);
		for (std::vector<math::Vector>::iterator it2 = values.begin(); it2 != values.end(); it2++) {
			//Same vector
			if (it->getX() == it2->getX() && it->getY() == it2->getY())
				continue;
			//Entry is not added yet, Entry contains a valid value and distance is small enough
			if (it2->getGroup() == -1 && it2->getZ() != NAN &&
				math::Vector::distance(*it2, point, 2) < precision) {
				it2->setGroup(0);
				it->setGroup(0);
				result.insert(std::make_pair(it->getZ(), it2->getZ()));
				BOOST_LOG_TRIVIAL(trace) << "connected " << *it << " with " << *it2;
				break;
			}
		}
	}
	return result;
}