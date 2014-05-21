#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/EMGProvider.h"

//sets the value of bufferWarnSize to 5, due to forbidden 'in-class initialization'.
int EMGProvider::bufferWarnSize = 5;

EMGProvider::~EMGProvider() {
	BOOST_LOG_TRIVIAL(info) << "Deleting all stored Intervals. This may take some time...";
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
	while (!intervals.empty())
		delete intervals.pop();
	BOOST_LOG_TRIVIAL(info) << "Deleted all stored Intervals";
}

int EMGProvider::getBufferSize() const {
	return intervals.size();
}