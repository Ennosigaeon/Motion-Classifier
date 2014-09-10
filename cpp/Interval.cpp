#include <cmath>
#include <ctime>
#include <boost/lexical_cast.hpp>
#include "../h/AppConfig.h"
#include "../h/Interval.h"
#include "../h/Logger.h"

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
	Logger::getInstance()->trace("calculating mean Sample");
	if (mean != NULL)
		return mean;

	clock_t t = clock();
	mean = calcMean(samples);
	t = clock() - t;
	Logger::getInstance()->trace("mean calculation took " + boost::lexical_cast<std::string>(((double)t) / CLOCKS_PER_SEC * 1000) + " ms");

	return mean;
}

void Interval::setMeanFunction(Sample* (*pointer)(const std::vector<Sample*>& values)) {
	calcMean = pointer;
}

const std::vector<Sample*>& Interval::getSamples() const {
	return samples;
}

void Interval::setGroup(const int group) {
	Interval::group = group;
}

int Interval::getGroup() const {
	return group;
}