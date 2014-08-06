#include "../h/Sample.h"
#include "../h/Means.h"

using namespace motion_classifier;

Sample* motion_classifier::math::getRMSMean(const std::vector<Sample*>& values) {
	if (values.empty())
		return NULL;

	int size = values.at(0)->getSize();
	//creates both arrays and inititalizes them with 0
	double *value = new double[size]();
	int *count = new int[size]();

	for (Sample *sample : values) {
		math::Vector *entries = sample->getEntries();
		for (int i = 0; i < size; ++i) {
			double n = entries[i].get(math::Dimension::Z);
			if (!isnan(n)) {
				value[i] += n * n;
				++count[i];
			}
		}
	}

	for (int i = 0; i < size; ++i)
		value[i] = sqrt(value[i] / count[i]);
	
	Sample *result = new Sample(value, size);

	delete[] value;
	delete[] count;
	return result;
}

Sample* math::getArithmeticMean(const std::vector<Sample*>& values) {
	if (values.empty())
		return NULL;

	int size = values.at(0)->getSize();
	//creates both arrays and inititalizes them with 0
	double *value = new double[size]();
	int *count = new int[size]();

	for (Sample *sample : values) {
		math::Vector *entries = sample->getEntries();
		for (int i = 0; i < size; ++i) {
			double n = entries[i].get(math::Dimension::Z);
			if (!isnan(n)) {
				value[i] += n;
				++count[i];
			}
		}
	}

	for (int i = 0; i < size; ++i)
		value[i] /= count[i];

	Sample *result = new Sample(value, size);

	delete[] value;
	delete[] count;
	return result;
}

Sample* math::getGeometricMean(const std::vector<Sample*>& values) {
	if (values.empty())
		return NULL;

	int size = values.at(0)->getSize();
	//creates both arrays and inititalizes them with 0
	double *value = new double[size]();
	int *count = new int[size]();

	for (Sample *sample : values) {
		math::Vector *entries = sample->getEntries();
		for (int i = 0; i < size; ++i) {
			double n = entries[i].get(math::Dimension::Z);
			if (!isnan(n)) {
				if (count[i] == 0)
					value[i] = n;
				else
					value[i] *= n;
				++count[i];
			}
		}
	}

	for (int i = 0; i < size; ++i)
		value[i] = pow(value[i], 1.0 / count[i]);

	Sample *result = new Sample(value, size);

	delete[] value;
	delete[] count;
	return result;
}

Sample* math::getHarmonicMean(const std::vector<Sample*>& values) {
	if (values.empty())
		return NULL;

	int size = values.at(0)->getSize();
	//creates both arrays and inititalizes them with 0
	double *value = new double[size]();
	int *count = new int[size]();

	for (Sample *sample : values) {
		math::Vector *entries = sample->getEntries();
		for (int i = 0; i < size; ++i) {
			double n = entries[i].get(math::Dimension::Z);
			if (!isnan(n)) {
				value[i] += 1.0 / n;
				++count[i];
			}
		}
	}

	for (int i = 0; i < size; ++i)
		value[i] = count[i] / value[i];

	Sample *result = new Sample(value, size);

	delete[] value;
	delete[] count;
	return result;
}
