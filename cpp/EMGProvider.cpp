#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "../h/EMGProvider.h"

int EMGProvider::bufferWarnSize = 5;

EMGProvider::~EMGProvider() {
	BOOST_LOG_TRIVIAL(info) << "Deleting all stored Intervals. This may take some time...";
	if (lastInterval != NULL)
		delete lastInterval;
	clearBuffer();
}

Interval* EMGProvider::getInterval() {
	Interval *interval = intervals.pop();
	//EMGProvider creates Intervals much faster, than they are processed
	if (intervals.size() > bufferWarnSize)
		BOOST_LOG_TRIVIAL(warning) << intervals.size() << " Intervals are stored in EMGProvider.";
	return interval;
}

void EMGProvider::addInterval(Interval* const interval) {
	intervals.push(interval);
}

void EMGProvider::clearBuffer() {
	while (!intervals.empty())
		delete intervals.pop();
	BOOST_LOG_TRIVIAL(info) << "Deleted all stored Intervals";
}