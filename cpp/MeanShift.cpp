#include <ctime>
#include <boost/lexical_cast.hpp>
#include "../h/MeanShift.h"
#include "../h/Logger.h"

using namespace motion_classifier;

MeanShift::MeanShift(math::KernelType kernel, math::Space *space, double epsilon, double maxCenterDist) {
	MeanShift::kernel = new math::Kernel(kernel);
	MeanShift::epsilon = epsilon;
	MeanShift::maxCenterDist = maxCenterDist;
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
	dataPoints = input;
}

std::vector<math::Vector*>* MeanShift::calculate(double h) {
	if (dataPoints == NULL)
		throw std::invalid_argument("No data points defined");
	if (h < 0)
		throw std::invalid_argument("Bandwidth is negative. Only positive values are allowed.");

	clock_t t = clock();
	std::vector<math::Vector*> *centers = new std::vector<math::Vector*>;
	int size = dataPoints->getNrRows() * dataPoints->getNrColumns();
	math::Vector *vectors = dataPoints->getEntries();
	for (int i = 0; i < size; ++i) {
		math::Vector x(vectors[i]);
		do {
			math::Vector numerator;
			double denomiator = 0;
			for (int j = 0; j < size; ++j) {
				double val = kernel->calculate(pow((x - vectors[j]).getLength() / h, 2));
				numerator += vectors[j] * val;
				denomiator += val;
			}
			double offset = ((numerator / denomiator) - x).getLength();
			x = numerator / denomiator;

			if (offset < epsilon)
				break;
		} while (true);

		math::Vector *closest = NULL;
		//TODO: make configurable
		//center has to be closer then predefined value
		double minDist = MeanShift::maxCenterDist;
		for (auto it2 = centers->begin(); it2 != centers->end(); ++it2) {
			double dist = (**it2 - x).getLength();
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