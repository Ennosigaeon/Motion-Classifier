
#include <array>
#include "../h/Space.h"

using namespace motion_classifier;

math::Norm math::Space::norm = math::Norm::L2;

math::Space::Space(double minX, double maxX, double minY, double maxY) {
	Space::min[0] = minX;
	Space::min[1] = minY;
	Space::max[0] = maxX;
	Space::max[1] = maxY;
}

math::Space::~Space() {
	delete[] min;
	delete[] max;
}

void math::Space::setNorm(math::Norm norm) {
	Space::norm = norm;
}

double* math::Space::addition(const double *left, const double *right) {
	double *res = new double[DIMENSIONS];
	for (unsigned int i = 0; i < DIMENSIONS; ++i)
		res[i] = left[i] + right[i];
	return normalize(res);
}

double* math::Space::subtraction(const double *left, const double *right) {
	double *res = new double[DIMENSIONS];
	for (unsigned int i = 0; i < DIMENSIONS; ++i)
		res[i] = left[i] - right[i];
	return normalize(res);
}

double* math::Space::multiplication(const double *left, double value) {
	double *res = new double[DIMENSIONS];
	for (unsigned int i = 0; i < DIMENSIONS; ++i)
		res[i] = left[i] * value;
	return normalize(res);
}

double* math::Space::division(const double *left, double value) {
	double *res = new double[DIMENSIONS];
	for (unsigned int i = 0; i < DIMENSIONS; ++i)
		res[i] = left[i] / value;
	return normalize(res);
}

double* math::Space::normalize(double *values) {
	for (int i = 0; i < 2; ++i) {
		if (values[i] < min[i])
			values[i] = max[i] - abs(min[i] - values[i]);
		if (values[i] > max[i])
			values[i] = min[i] + abs(values[i] - max[i]);
	}
	return values;
}

double math::Space::distance(const double *left, const double *right) {
	std::array<double*, 5> vectors;
	double d0[3] = { right[0], right[1], right[2] };
	double d1[3] = { right[0] + (max[0] - min[0]), right[1], right[2] };
	double d2[3] = { right[0] - (max[0] - min[0]), right[1], right[2] };
	double d3[3] = { right[0], right[1] + (max[1] - min[1]), right[2] };
	double d4[3] = { right[0], right[1] - (max[1] - min[1]), right[2] };
	vectors[0] = d0;
	vectors[1] = d1;
	vectors[2] = d2;
	vectors[3] = d3;
	vectors[4] = d4;

	double minLength = std::numeric_limits<double>::max();
	for (double *vec : vectors) {
		double *d = subtraction(left, vec);
		double l = length(d);
		if (l < minLength)
			minLength = l;
		delete d;
	}

	return minLength;
}

double math::Space::length(const double *values, const int nrDimensions) {
	switch (norm) {
	case math::Norm::L1: {
		double value = 0;
		for (int i = 0; i < nrDimensions; ++i)
			value += abs(values[i]);
		return value;
	}
	case math::Norm::L2:
	default: {
		double value = 0;
		for (int i = 0; i < nrDimensions; ++i)
			value += values[i] * values[i];
		return sqrt(value);
	}
	}
}
