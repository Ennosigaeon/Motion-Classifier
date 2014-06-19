#ifndef INTERVAL_H
#define INTERVAL_H

#include "Means.h"
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
		Sample *mean = NULL;
		int maxNrSamples;

		static Sample* (*calcMean)(const std::vector<Sample*>& values);

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
		  * Calculates the Mean over all stored samples. You
		  * can specify the concrete mean function by calling
		  * setMeanFunction(). Possible functions are defined
		  * in Means.h. The result is cached, so that a second
		  * call will not result in a second calculation of
		  * the mean. Returns NULL when no Samples are stored
		  * in this Interval.
		  */
		Sample* getMeanSample();

		/**
		  * Sets the mean function that is used in getMeanSquare().
		  * See Means.h for some examples of possible functions. The
		  * default mean function calculates the Root Mean Square.
		  * The mean function will be changed for all Intervals.
		  */
		void static setMeanFunction(Sample* (*pointer)(const std::vector<Sample*>& values));

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