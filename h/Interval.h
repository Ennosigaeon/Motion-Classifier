#ifndef INTERVAL_H
#define INTERVAL_H

#include "Sample.h"

/**
  * This class contains all Samples of a certain time amount. The actual
  * timeamount is pretty important. When the interval is too long, the accuracy
  * of the classification will drop. When the interval is too short, the
  * classification takes too much time and the motion will not be evaluated in
  * real time.
  * It is planned to implement a real time controler that adapts the maximum size
  * on the fly. In the mean time the number of samples is set by the configuration
  * file.
  */
class Interval {
private:
	std::vector<Sample*> samples;
	Sample *rms = NULL;
	int MAX_SIZE;

public:
	/**
	  * Creates a new empty Interval.
	  */
	Interval();
	~Interval();

	/**
	  * Tests whether this Interval is full (maximum number of samples stored) or not.
	  */
	inline bool isFull() const;

	/**
	  * Calculates the Root Mean Square (RMS) over all stored samples. The result
	  * is cached. Returns NULL when no Samples are stored in this Interval.
	  */
	Sample* getRMSSample();

	/**
	  * Adds a new Sample to this Interval. It is crucial that all Samples
	  * in an Interval have the same number of rows and columns.
	  */
	void addSample(Sample *sample);

	const std::vector<Sample*>& getSamples() const;
};

#endif