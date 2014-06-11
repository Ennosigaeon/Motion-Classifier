#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/AppConfig.h"
#include "../h/Utilities.h"
#include "../h/Variogram.h"

Variogram::Variogram() {
	nrBins = AppConfig::getInstance()->getVariogramNrBins();
}

std::vector<math::Vector> Variogram::calculate(Sample* sample) const {
	std::vector<math::Vector> result;

	int maxX = sample->getNrColumns() / 2;
	int maxY = sample->getNrRows() / 2;
	double precision = std::max(maxX, maxY) * 1.0 / nrBins;
	clock_t t = clock();

	for (int i = math::Angle::DEGREE_0; i <= math::Angle::DEGREE_150; ++i) {
		math::Angle angle = static_cast<math::Angle>(i);
		math::Vector h = math::Vector::getVector(angle);
		int count = 0;

		while (std::abs(h.get(0)) <= maxX && std::abs(h.get(1)) <= maxY) {
			int c = 0;
			double value = calc(sample, h, precision, &c);
			count += c;

			if (!isnan(value)) {
				math::Vector tmp = h;
				tmp.setGroup(angle);
				tmp.setZ(value);
				result.push_back(tmp);
			}
			h.setLength(h.getLength() + 1);
		}
		BOOST_LOG_TRIVIAL(trace) << "found " << count << " pairs for math::Angle " << printAngle(angle);
	}
	t = clock() - t;
	BOOST_LOG_TRIVIAL(debug) << "Variogram calculation took " << ((double)t) / CLOCKS_PER_SEC * 1000 << " ms. " << result.size() << " pairs found";

	return result;
}

double Variogram::calc(Sample* sample, const math::Vector& h, const double radius, int *count) const {
	double result = 0;
	int size = sample->getNrColumns() * sample->getNrRows();
	math::Vector *entries = sample->getEntries();

	for (int i = 0; i < size; ++i) {
		if (entries[i].getGroup() != -1 || isnan(entries[i].getZ()))
			continue;
		math::Vector point(entries[i].getX() + h.getX(), entries[i].getY() + h.getY());


		int endX = std::min(floor(point.getX() + radius), sample->getNrColumns() - 1 * 1.0);
		int endY = std::min(floor(point.getY() + radius), sample->getNrRows() - 1 * 1.0);
		for (int x = std::max(ceil(point.getX() - radius), 0.0); x <= endX; ++x) {
			for (int y = std::max(ceil(point.getY() - radius), 0.0); y <= endY; ++y) {
				int index = y + x * sample->getNrRows();
				if (index != i && entries[index].getGroup() == -1 &&
					pow(x - point.getX(), 2) + pow(y - point.getY(), 2) <= radius * radius) {
					entries[index].setGroup(0);
					entries[i].setGroup(0);
					result += pow(entries[i].getZ() - entries[index].getZ(), 2);
					++(*count);
					goto end;
				}
			}
		}
	//Used to break nested loops
	end:;
	}

	//reset for next run
	for (int i = 0; i < size; ++i)
		entries[i].setGroup(-1);

	BOOST_LOG_TRIVIAL(trace) << "found " << *count << " pairs for the offset " << h;
	if (*count == 0)
		return NAN;
	else
		return result / (2 * (*count));
}