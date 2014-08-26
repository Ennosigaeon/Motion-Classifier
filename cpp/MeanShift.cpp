#include <ctime>
#include <boost/lexical_cast.hpp>
#include <thread>
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

void MeanShift::calc(int start, int end, double h, std::vector<math::Vector*> *result) {

	for (int i = start; i < end; ++i) {
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
		for (auto &center : *result) {
			double dist = x.getDistance(*center);
			if (dist < h) {
				closest = center;
				break;
			}
		}
		if (closest == NULL)
			result->push_back(new math::Vector(x));
	}
}

std::vector<math::Vector*>* MeanShift::calculate(double h) {
	if (input == NULL)
		throw std::invalid_argument("No data points defined");
	if (h < 0)
		throw std::invalid_argument("Bandwidth is negative. Only positive values are allowed.");

	clock_t t = clock();
	std::vector<math::Vector*> *centers = new std::vector<math::Vector*>;

	int middle = size / 2;

	std::vector<math::Vector*> centers1;
	std::vector<math::Vector*> centers2;
	std::thread t1(&MeanShift::calc, this, 0, middle, h, &centers1);
	std::thread t2(&MeanShift::calc, this, middle + 1, size, h, &centers2);
	t1.join();
	t2.join();

	centers->insert(centers->end(), centers1.begin(), centers1.end());
	centers->insert(centers->end(), centers2.begin(), centers2.end());

	//remove possible duplicates
	for (auto it = centers->begin(); it != centers->end();) {
		if ((**it).getGroup() == 1)
			it = centers->erase(it);
		else {
			auto it2 = it;
			for (it2++; it2 != centers->end(); ++it2) {
				if ((**it2).getGroup() == 1)
					continue;
				double dist = (**it2).getDistance(**it);
				if (dist < h)
					(**it2).setGroup(1);
			}
			++it;
		}
	}

	t = clock() - t;
	Logger::getInstance()->trace("Mean Shift took " + boost::lexical_cast<std::string>(((double)t) / CLOCKS_PER_SEC * 1000) + " ms");
	return centers;
}
