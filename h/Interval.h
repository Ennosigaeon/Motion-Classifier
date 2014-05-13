
#ifndef INTERVAL_H
#define INTERVAL_H

#include "Sample.h"

class Interval {
private:
	std::vector<Sample> samples;
	int MAX_SIZE;

public:
	Interval();

	bool isFull() const;
	// Calculates the Root Mean Square over all stored samples. The result is not cached.
	Sample getRMSSample();
	std::vector<Sample> getSamples() const;
	void addSample(const Sample& sample);
};

#endif