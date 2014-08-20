#include <ctime>
#include <boost/lexical_cast.hpp>
#include "../h/MeanShift.h"
#include "../h/Logger.h"

using namespace motion_classifier;

MeanShift::MeanShift(math::KernelType kernel, double epsilon, double threshold) {
	MeanShift::kernel = new math::Kernel(kernel);
	MeanShift::epsilon = epsilon;
	MeanShift::threshold = threshold;
}

MeanShift::~MeanShift() {
	delete kernel;
}

void MeanShift::setDataPoints(math::Vector* input, int size) {
	MeanShift::input = input;
	MeanShift::size = size;
}

void MeanShift::setFiltering(bool doFilter) {
	MeanShift::filter = doFilter;
}

std::vector<math::Vector*>* MeanShift::calculate(double h) {
	if (input == NULL)
		throw std::invalid_argument("No data points defined");
	if (h < 0)
		throw std::invalid_argument("Bandwidth is negative. Only positive values are allowed.");

	clock_t t = clock();
	std::vector<math::Vector*> *centers = new std::vector<math::Vector*>;
	for (int i = 0; i < size; ++i) {
		if (isnan(input[i].get(math::Dimension::Z)))
			continue;

		math::Vector x(input[i]);
		//If EMG value is samller then threshold, it is ignored => Rest Position has no centers
		if (x.get(math::Dimension::Z) < threshold)
			continue;

		double offset;
		do {
			math::Vector numerator;
			double denomiator = 0;
			for (int j = 0; j < size; ++j) {
				if (isnan(input[j].get(math::Dimension::Z)))
					continue;

				double val = x.getDistance(input[j]) / h;
				val = kernel->calculate(val * val);
				numerator += input[j] * val;
				denomiator += val;
			}
			math::Vector tmp = numerator / denomiator;
			offset = x.getDistance(tmp);
			x = tmp;
		} while (offset > epsilon);

		math::Vector *closest = NULL;
		//center has to be closer then predefined value
		double minDist = h;
		for (auto &center : *centers) {
			double dist = x.getDistance(*center);
			if (dist < minDist) {
				closest = center;
				minDist = dist;
			}
		}
		if (closest == NULL) {
			closest = new math::Vector(x);
			closest->setGroup(centers->size());
			centers->push_back(closest);
		}

		input[i].setGroup(closest->getGroup());
		if (filter)
			input[i].set(math::Dimension::Z, closest->get(math::Dimension::Z));
	}

	t = clock() - t;
	Logger::getInstance()->trace("Mean Shift took " + boost::lexical_cast<std::string>(((double)t) / CLOCKS_PER_SEC * 1000) + " ms");

	return centers;
}
