#include <ctime>
#include <boost/lexical_cast.hpp>
#include "../h/MeanShift.h"
#include "../h/Logger.h"

using namespace motion_classifier;

MeanShift::MeanShift(math::KernelType kernel, math::Space *space, double epsilon, double threshold) {
	MeanShift::kernel = new math::Kernel(kernel);
	MeanShift::epsilon = epsilon;
	MeanShift::threshold = threshold;
	setSpace(space);
}

MeanShift::~MeanShift() {
	if (kernel != NULL)
		delete kernel;
}

void MeanShift::setSpace(math::Space *space) {
	math::Vector::setSpace(space);
}

void MeanShift::setDataPoints(Sample *input) {
	sample = input;
}

std::vector<math::Vector*>* MeanShift::calculate(double h) {
	if (sample == NULL)
		throw std::invalid_argument("No data points defined");
	if (h < 0)
		throw std::invalid_argument("Bandwidth is negative. Only positive values are allowed.");

	clock_t t = clock();
	std::vector<math::Vector*> *centers = new std::vector<math::Vector*>;
	int size = sample->getNrRows() * sample->getNrColumns();
	math::Vector *vectors = sample->getEntries();
	for (int i = 0; i < size; ++i) {
		math::Vector x(vectors[i]);

		//If EMG value is samller then threshold, it is ignored => Rest Position has no centers
		if (x.get(math::Dimension::Z) < threshold)
			continue;

		double offset;
		do {
			math::Vector numerator;
			double denomiator = 0;
			for (int j = 0; j < size; ++j) {
				double val = x.getDistance(vectors[j]) / h;
				val = kernel->calculate(val * val);
				numerator += vectors[j] * val;
				denomiator += val;
			}
			math::Vector tmp = numerator / denomiator;
			offset = x.getDistance(tmp);
			x = tmp;
		} while (offset < epsilon);

		math::Vector *closest = NULL;
		//center has to be closer then predefined value
		double minDist = h;
		for (auto it2 = centers->begin(); it2 != centers->end(); ++it2) {
			double dist = x.getDistance(**it2);
			if (dist < minDist) {
				closest = *it2;
				minDist = dist;
			}
		}
		if (closest == NULL) {
			math::Vector *vec = new math::Vector(x);
			vec->setGroup(centers->size());
			vectors[i].setGroup(vec->getGroup());
			centers->push_back(vec);
		}
		else
			vectors[i].setGroup(closest->getGroup());
	}

	t = clock() - t;
	Logger::getInstance()->debug("classification took " + boost::lexical_cast<std::string>(((double)t) / CLOCKS_PER_SEC * 1000) + " ms");

	return centers;
}

void MeanShift::filter(std::vector<math::Vector*> *centers) {
	for (math::Vector *center : *centers) {
		for (int i = 0; i < sample->getNrRows() * sample->getNrColumns(); ++i) {
			math::Vector *vec = &sample->getEntries()[i];
			if (vec->getGroup() == center->getGroup())
				vec->set(math::Dimension::Z, center->get(math::Dimension::Z));
		}
	}
}