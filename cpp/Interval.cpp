#include <cmath>
#include <ctime>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/AppConfig.h"
#include "../h/Interval.h"

using namespace motion_classifier;

Sample* (*Interval::calcMean)(const std::vector<Sample*>& values) = &math::getRMSMean;

Interval::Interval() {
	maxNrSamples = AppConfig::getInstance()->getIntervalNrSamples();
}

Interval::~Interval() {
	for (std::vector<Sample*>::iterator it = samples.begin(); it != samples.end(); ++it)
		delete *it;
	if (mean != NULL)
		delete mean;
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

Sample* Interval::getMeanSample() {
	BOOST_LOG_TRIVIAL(trace) << "calculating mean Sample";
	if (mean != NULL)
		return mean;

	clock_t t = clock();
	mean = calcMean(samples);
	t = clock() - t;
	BOOST_LOG_TRIVIAL(debug) << "mean calculation took " << ((double)t) / CLOCKS_PER_SEC * 1000 << " ms";

	return mean;
}

void Interval::setMeanFunction(Sample* (*pointer)(const std::vector<Sample*>& values)) {
	calcMean = pointer;
}

const std::vector<Sample*>& Interval::getSamples() const {
	return samples;
}