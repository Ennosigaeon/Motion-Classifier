#include <cmath>
#include <ctime>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/AppConfig.h"
#include "../h/Interval.h"

using namespace motion_classifier;

Interval::Interval() {
	maxNrSamples = AppConfig::getInstance()->getIntervalNrSamples();
}

Interval::~Interval() {
	for (std::vector<Sample*>::iterator it = samples.begin(); it != samples.end(); ++it)
		delete *it;
	if (rms != NULL)
		delete rms;
}

inline bool Interval::isFull() const {
	return samples.size() == maxNrSamples;
}

void Interval::addSample(Sample* sample) {
	//it is not possible to add Samples, when the Interval is full
	if (isFull())
		return;
	samples.push_back(sample);
}

Sample* Interval::getRMSSample() {
	BOOST_LOG_TRIVIAL(trace) << "calculating RMS Sample";
	if (samples.empty())
		return NULL;
	if (rms != NULL)
		return rms;

	clock_t t = clock();
	rms = new Sample();
	int size = rms->getNrRows() * rms->getNrColumns();

	//creates both arrays and inititalizes them with 0
	double *value = new double[size]();
	int *count = new int[size]();

	for (std::vector<Sample*>::iterator it = samples.begin(); it != samples.end(); ++it) {
		math::Vector* entries = (*it)->getEntries();
		for (int i = 0; i < size; ++i) {
			double n = entries[i].getZ();
			if (!isnan(n)) {
				value[i] += n * n;
				++count[i];
			}
		}
	}
	math::Vector* entries = samples.at(0)->getEntries();
	math::Vector* resultEntries = rms->getEntries();
	for (int i = 0; i < size; ++i)
		resultEntries[i] = math::Vector(entries[i].getX(), entries[i].getY(), sqrt(value[i] / count[i]));

	delete[] value;
	delete[] count;

	t = clock() - t;
	BOOST_LOG_TRIVIAL(debug) << "RMS calculation took " << ((double)t) / CLOCKS_PER_SEC * 1000 << " ms";

	return rms;
}

const std::vector<Sample*>& Interval::getSamples() const {
	return samples;
}