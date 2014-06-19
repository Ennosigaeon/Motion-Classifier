#include "../h/Sample.h"
#include "../h/Means.h"

using namespace motion_classifier;

Sample* motion_classifier::math::getRMSMean(const std::vector<Sample*>& values) {
	if (values.empty())
		return NULL;

	Sample *result = new Sample();
	int size = result->getNrRows() * result->getNrColumns();

	//creates both arrays and inititalizes them with 0
	double *value = new double[size]();
	int *count = new int[size]();

	for (std::vector<Sample*>::const_iterator it = values.begin(); it != values.end(); ++it) {
		math::Vector* entries = (*it)->getEntries();
		for (int i = 0; i < size; ++i) {
			double n = entries[i].getZ();
			if (!isnan(n)) {
				value[i] += n * n;
				++count[i];
			}
		}
	}
	math::Vector* entries = values.at(0)->getEntries();
	math::Vector* resultEntries = result->getEntries();
	for (int i = 0; i < size; ++i)
		resultEntries[i] = math::Vector(entries[i].getX(), entries[i].getY(), sqrt(value[i] / count[i]));

	delete[] value;
	delete[] count;
	return result;
}

Sample* math::getArithmeticMean(const std::vector<Sample*>& values) {
	if (values.empty())
		return NULL;

	Sample *result = new Sample();
	int size = result->getNrRows() * result->getNrColumns();

	//creates both arrays and inititalizes them with 0
	double *value = new double[size]();
	int *count = new int[size]();

	for (std::vector<Sample*>::const_iterator it = values.begin(); it != values.end(); ++it) {
		math::Vector* entries = (*it)->getEntries();
		for (int i = 0; i < size; ++i) {
			double n = entries[i].getZ();
			if (!isnan(n)) {
				value[i] += n;
				++count[i];
			}
		}
	}
	math::Vector* entries = values.at(0)->getEntries();
	math::Vector* resultEntries = result->getEntries();
	for (int i = 0; i < size; ++i)
		resultEntries[i] = math::Vector(entries[i].getX(), entries[i].getY(), value[i] / count[i]);

	delete[] value;
	delete[] count;
	return result;
}

Sample* math::getGeometricMean(const std::vector<Sample*>& values) {
	if (values.empty())
		return NULL;

	Sample *result = new Sample();
	int size = result->getNrRows() * result->getNrColumns();

	//creates both arrays and inititalizes them with 0
	double *value = new double[size]();
	int *count = new int[size]();

	for (std::vector<Sample*>::const_iterator it = values.begin(); it != values.end(); ++it) {
		math::Vector* entries = (*it)->getEntries();
		for (int i = 0; i < size; ++i) {
			double n = entries[i].getZ();
			if (!isnan(n)) {
				if (count[i] == 0)
					value[i] = n;
				else
					value[i] *= n;
				++count[i];
			}
		}
	}
	math::Vector* entries = values.at(0)->getEntries();
	math::Vector* resultEntries = result->getEntries();
	for (int i = 0; i < size; ++i)
		resultEntries[i] = math::Vector(entries[i].getX(), entries[i].getY(), pow(value[i], 1.0 / count[i]));

	delete[] value;
	delete[] count;
	return result;
}

Sample* math::getHarmonicMean(const std::vector<Sample*>& values) {
	if (values.empty())
		return NULL;

	Sample *result = new Sample();
	int size = result->getNrRows() * result->getNrColumns();

	//creates both arrays and inititalizes them with 0
	double *value = new double[size]();
	int *count = new int[size]();

	for (std::vector<Sample*>::const_iterator it = values.begin(); it != values.end(); ++it) {
		math::Vector* entries = (*it)->getEntries();
		for (int i = 0; i < size; ++i) {
			double n = entries[i].getZ();
			if (!isnan(n)) {
				value[i] += 1.0 / n;
				++count[i];
			}
		}
	}
	math::Vector* entries = values.at(0)->getEntries();
	math::Vector* resultEntries = result->getEntries();
	for (int i = 0; i < size; ++i)
		resultEntries[i] = math::Vector(entries[i].getX(), entries[i].getY(), count[i] / value[i]);

	delete[] value;
	delete[] count;
	return result;
}