#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/EMGProviderImpl.h"

using namespace motion_classifier;

EMGProviderImpl::~EMGProviderImpl() {
	//TODO: consumes a lot of time
	if (lastInterval != NULL)
		delete lastInterval;
	clearBuffer();
}

Interval* EMGProviderImpl::getInterval() {
	Interval *interval = intervals.pop();
	return interval;
}

void EMGProviderImpl::addInterval(Interval* const interval) {
	intervals.push(interval);

	//EMGProvider could create Intervals much faster, than they are processed
	if (intervals.size() > bufferWarnSize)
		BOOST_LOG_TRIVIAL(warning) << intervals.size() << " Intervals are stored in EMGProvider";
}

void EMGProviderImpl::clearBuffer() {
	//This operation can take a lot of time (>30 seconds).
	BOOST_LOG_TRIVIAL(info) << "Deleting " << intervals.size() << " stored Intervals. This may take some time...";
	while (!intervals.empty())
		delete intervals.pop();
}

int EMGProviderImpl::getBufferSize() const {
	return intervals.size();
}

int EMGProviderImpl::getSampleNr() const {
	return sampleNr;
}