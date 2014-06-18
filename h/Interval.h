#ifndef INTERVAL_H
#define INTERVAL_H

#include "Sample.h"

namespace motion_classifier {
	/**
	  * This class contains all recorded Samples during a specific
	  * time. The actual timeamount can be implicitly defined via
	  * the configuration file (interval.nrSamples). The EMGProvider
	  * creates Intervals, it is the smallest collection of EMG
	  * signals to work with in a reasonable way.
	  */
	class Interval {
	private:
		std::vector<Sample*> samples;
		Sample *rms = NULL;
		int maxNrSamples;

	public:
		/**
		  * Creates a new empty Interval.
		  */
		Interval();

		/**
		  * Destroys the Interval. All Samples will be deleted.
		  */
		~Interval();

		/**
		  * Tests whether this Interval is full (maximum number of
		  * samples stored) or not.
		  */
		inline bool isFull() const;

		/**
		  * Calculates the Root Mean Square (RMS) over all stored
		  * samples. The result is cached, so that a second call
		  * will not result in a second calculation of the RMS.
		  * Returns NULL when no Samples are stored in this Interval.
		  */
		Sample* getRMSSample();

		/**
		  * Adds a new Sample to this Interval. It is crucial that
		  * all Samples in an Interval have the same number of rows
		  * and columns. When the Interval is already full the Sample
		  * will not be entered.
		  */
		void addSample(Sample *sample);

		/**
		  * Returns a std::vector with all stored Samples in this
		  * Interval.
		  */
		const std::vector<Sample*>& getSamples() const;
	};
}

#endif