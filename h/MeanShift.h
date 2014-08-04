#ifndef MEANSHIFT_H
#define MEANSHIFT_H

#include <vector>
#include "Sample.h"
#include "Space.h"
#include "Kernel.h"
#include "Vector.h"

namespace motion_classifier {

	class MeanShift {
	private:
		math::Kernel *kernel = NULL;
		Sample *sample = NULL;

		double epsilon;
		double threshold;

	public:
		MeanShift(math::KernelType kernel, math::Space *space, double epsilon = 0.05, double threshold = 30);

		~MeanShift();

		/**
		  * Sets the math::Space for calculating distances.
		  */
		void setSpace(math::Space *space);

		/**
		  * Sets the data points for the Mean Shift procedure. Use
		  * MeanShift::calculate(double, double, int) to calculate the clusters.
		  */
		void setDataPoints(Sample *sample);

		/**
		  * Performs the Mean Shift algorithm on the given data points (see setDataPoints).
		  * Each math::Vector is assigned to a cluster (group). In addition all center
		  * of clusters are returned. The index in the returned list corresponds to
		  * the cluster number.
		  * h:	bandwidth
		  */
		std::vector<math::Vector*>* calculate(double h);

		/**
		  * Filters the given data points (see setDataPoints) with the given centers. 
		  * All math::Vectors assigned to a given center (via its group and index) get
		  * the same z value as the center.
		  */
		void filter(std::vector<math::Vector*> *centers);
	};
}

#endif