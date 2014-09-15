#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "../h/AppConfig.h"
#include "../h/Logger.h"
#include "../h/Utilities.h"
#include "../h/Variogram.h"

using namespace motion_classifier;

Variogram::Variogram(int nrBins) {
	Variogram::nrBins = nrBins;
}

std::vector<math::Vector>* Variogram::calculate(Sample* sample) const {
	Logger *logger = Logger::getInstance();
	auto *result = new std::vector<math::Vector>;

	int maxX = sample->getNrColumns() / 2;
	int maxY = sample->getNrRows() / 2;
	double precision = std::max(maxX, maxY) * 1.0 / nrBins;
	clock_t t = clock();

	for (int i = math::Angle::DEGREE_0; i <= math::Angle::DEGREE_150; ++i) {
		math::Angle angle = static_cast<math::Angle>(i);
		math::Vector h = math::Vector::getVector(angle);
		int count = 0;

		while (std::abs(h.get(math::Dimension::X)) <= maxX && std::abs(h.get(math::Dimension::Y)) <= maxY) {
			int c = 0;
			double value = calc(sample, h, precision, &c);
			count += c;

			if (!isnan(value)) {
				math::Vector tmp = h;
				tmp.setGroup(angle);
				tmp.set(math::Dimension::Z, value);
				result->push_back(tmp);
			}
			h.setLength(h.getLength() + 1);
		}
		logger->trace("found " + boost::lexical_cast<std::string>(count) + " pairs for math::Angle " + printAngle(angle));
	}
	t = clock() - t;
	logger->debug("Variogram calculation took " + boost::lexical_cast<std::string>(((double)t) / CLOCKS_PER_SEC * 1000) + " ms. " + boost::lexical_cast<std::string>(result->size()) + " pairs found");

	return result;
}

double Variogram::calc(Sample* sample, const math::Vector& h, const double radius, int *count) const {
	double result = 0;
	auto entries = sample->getEntries();

	for (int i = 0; i < sample->getSize(); ++i) {
		math::Vector &vector = entries[i];
		if (vector.getGroup() != -1 || isnan(vector.get(math::Dimension::Z)))
			continue;
		math::Vector point = vector + h;

		int endX = std::min(floor(point.get(0) + radius), sample->getNrColumns() - 1 * 1.0);
		int endY = std::min(floor(point.get(1) + radius), sample->getNrRows() - 1 * 1.0);
		for (int x = std::max(ceil(point.get(0) - radius), 0.0); x <= endX; ++x) {
			for (int y = std::max(ceil(point.get(1) - radius), 0.0); y <= endY; ++y) {
				int index = y + x * sample->getNrRows();
				math::Vector &vec = entries[index];
				if (vec != vector && vec.getGroup() == -1 &&
					pow(x - point.get(math::Dimension::X), 2) + pow(y - point.get(math::Dimension::Y), 2) <= radius * radius) {
					vec.setGroup(0);
					vector.setGroup(0);
					result += pow(vector.get(math::Dimension::Z) - vec.get(math::Dimension::Z), 2);
					++(*count);
					goto end;
				}
			}
		}
	//Used to break nested loops
	end:;
	}

	//reset for next run
	for (int i = 0; i < sample->getSize(); ++i)
		entries[i].setGroup(-1);

	Logger::getInstance()->trace("found " + boost::lexical_cast<std::string>(*count) + " pairs for the offset " + boost::lexical_cast<std::string>(h));
	if (*count == 0)
		return NAN;
	else
		return result / (2 * (*count));
}
