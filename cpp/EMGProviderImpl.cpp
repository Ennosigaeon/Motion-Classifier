#include <boost/lexical_cast.hpp>
#include "../h/EMGProviderImpl.h"
#include "../h/Logger.h"

using namespace motion_classifier;

EMGProviderImpl::~EMGProviderImpl() {
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
		Logger::getInstance()->warn(boost::lexical_cast<std::string>(intervals.size()) + " Intervals are stored in EMGProvider");
}

void EMGProviderImpl::clearBuffer() {
	//This operation can take a lot of time (>30 seconds).
	Logger::getInstance()->info("Deleting " + boost::lexical_cast<std::string>(intervals.size()) + " stored Intervals. This may take some time...");
	while (!intervals.empty())
		delete intervals.pop();
}

int EMGProviderImpl::getBufferSize() const {
	return intervals.size();
}

int EMGProviderImpl::getSampleNr() const {
	return sampleNr;
}