#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/EMGProvider.h"

//sets the value of bufferWarnSize to 5, due to forbidden 'in-class initialization'.
int EMGProvider::bufferWarnSize = 5;

EMGProvider::~EMGProvider() {
	if (lastInterval != NULL)
		delete lastInterval;
	clearBuffer();
}

Interval* EMGProvider::getInterval() {
	Interval *interval = intervals.pop();
	return interval;
}

void EMGProvider::addInterval(Interval* const interval) {
	intervals.push(interval);

	//EMGProvider creates Intervals much faster, than they are processed
	if (intervals.size() > bufferWarnSize)
		BOOST_LOG_TRIVIAL(warning) << intervals.size() << " Intervals are stored in EMGProvider";
}

void EMGProvider::clearBuffer() {
	//This operation can take a lot of time (>30 seconds).
	BOOST_LOG_TRIVIAL(info) << "Deleting " << intervals.size() << " stored Intervals. This may take some time...";
	while (!intervals.empty())
		delete intervals.pop();
}

int EMGProvider::getBufferSize() const {
	return intervals.size();
}

int EMGProvider::getSampleNr() const {
	return sampleNr;
}