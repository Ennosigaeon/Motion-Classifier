#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/AppConfig.h"
#include "../h/Variogram.h"

Variogram::Variogram() {
	nrBins = AppConfig::getInstance()->getVariogramNrBins();
}

std::vector<math::Vector> Variogram::calculate(Sample& sample) const {
	std::vector<math::Vector> result;

	int maxX = sample.getNrColumns() / 2;
	int maxY = sample.getNrRows() / 2;
	//distance is calculated without sqrt, therefore precision has to be squared
	double precision = pow(std::max(maxX, maxY) * 1.0 / nrBins, 2);
	clock_t t = clock();

	//TODO: math::Angle::DEGREE_150
	for (int i = math::Angle::DEGREE_0; i <= math::Angle::DEGREE_0; i++) {
		math::Angle angle = static_cast<math::Angle>(i);
		math::Vector h = math::Vector::getVector(angle);
		int count = 0;

		while (std::abs(h.get(0)) <= maxX && std::abs(h.get(1)) <= maxY) {
			std::map<double, double> pairs;
			findPairs(&pairs, sample.getEntries(), h, precision);
			count += pairs.size();

			BOOST_LOG_TRIVIAL(trace) << "found " << pairs.size() << " pairs for the offset " << h;
			if (!pairs.empty()) {
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
		BOOST_LOG_TRIVIAL(debug) << "found " << count << " pairs for math::Angle " << angle;
	}
	t = clock() - t;
	BOOST_LOG_TRIVIAL(debug) << "Variogram calculation took " << ((double)t) / CLOCKS_PER_SEC * 1000 << " ms. " << result.size() << " pairs found";

	return result;
}

//TODO: this is by far the slowest part of the application
void Variogram::findPairs(std::map<double, double> *result, std::vector<math::Vector>* values, const math::Vector& h, const double precision) const {
	for (std::vector<math::Vector>::iterator it = values->begin(); it != values->end(); it++) {
		if (it->getGroup() != -1)
			continue;
		math::Vector point(it->getX() + h.getX(), it->getY() + h.getY(), 0);
		for (std::vector<math::Vector>::iterator it2 = values->begin(); it2 != values->end(); it2++) {
			//Same vector
			if (it->getX() == it2->getX() && it->getY() == it2->getY())
				continue;
			//Entry is not added yet, Entry contains a valid value and distance is small enough
			if (it2->getGroup() == -1 && it2->getZ() != NAN &&
				math::Vector::distance(*it2, point, 2) < precision) {
				it2->setGroup(0);
				it->setGroup(0);
				result->insert(std::make_pair(it->getZ(), it2->getZ()));
				break;
			}
		}
	}
	for (std::vector<math::Vector>::iterator it = values->begin(); it != values->end(); it++)
		it->setGroup(-1);
}