
#include "../h/EMGProvider.h"

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
}

void EMGProvider::clearBuffer() {
	while (!intervals.empty())
		delete intervals.pop();
}