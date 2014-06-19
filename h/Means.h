#ifndef MEANS_H
#define MEANS_H

#include <vector>
#include "Sample.h"
#include "Vector.h"

namespace motion_classifier {
	namespace math {
		/**
		  * Calculates the Root Mean Square over the given
		  * Samples. If the collection is empty, NULL will
		  * be returned.
		  */
		Sample* getRMSMean(const std::vector<Sample*>& values);

		/**
		  * Calculates the Average over the given Samples. If
		  * the collection is empty, NULL will be returned.
		  */
		Sample* getArithmeticMean(const std::vector<Sample*>& values);

		/**
		  * Calculates the Geometric Mean over the given Samples.
		  * If the collection is empty, NULL will be returned.
		  */
		Sample* getGeometricMean(const std::vector<Sample*>& values);

		/**
		  * Calculates the Harmonic Mean over the given Samples.
		  * If the collection is empty, NULL will be returned.
		  */
		Sample* getHarmonicMean(const std::vector<Sample*>& values);
	}
}

#endif