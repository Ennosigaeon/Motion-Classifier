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

std::vector<math::Vector> Variogram::calculate(Sample* sample) const {
	std::vector<math::Vector> result;

	int maxX = sample->getNrColumns() / 2;
	int maxY = sample->getNrRows() / 2;
	//distance is calculated without sqrt, therefore precision has to be squared
	double precision = pow(std::max(maxX, maxY) * 1.0 / nrBins, 2);
	clock_t t = clock();

	for (int i = math::Angle::DEGREE_0; i <= math::Angle::DEGREE_150; i++) {
		math::Angle angle = static_cast<math::Angle>(i);
		math::Vector h = math::Vector::getVector(angle);
		int count = 0;

		while (std::abs(h.get(0)) <= maxX && std::abs(h.get(1)) <= maxY) {
			std::map<double, double> pairs;
			findPairs(&pairs, sample, h, precision);
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
void Variogram::findPairs(std::map<double, double> *result, Sample* sample, const math::Vector& h, const double precision) const {
	int size = sample->getNrColumns() * sample->getNrRows();
	math::Vector *entries = sample->getEntries();
	for (int i = 0; i < size; i++) {
		if (entries[i].getGroup() != -1 || entries[i].getZ() == NAN)
			continue;
		math::Vector point(entries[i].getX() + h.getX(), entries[i].getY() + h.getY(), 0);

		for (int j = 0; j < size; j++) {
			//Same vector
			if (entries[i].getX() == entries[j].getX() && entries[i].getY() == entries[j].getY())
				continue;
			if (entries[j].getGroup() == -1 && entries[j].getZ() != NAN &&
				math::Vector::distance(entries[j], point, 2) < precision) {
				entries[i].setGroup(0);
				entries[j].setGroup(0);
				//TODO: calculate value here
				result->insert(std::make_pair(entries[i].getZ(),entries[j].getZ()));
				break;
			}
		}
	}
	//reset for next run
	for (int i = 0; i < size; i++)
		entries[i].setGroup(-1);
}