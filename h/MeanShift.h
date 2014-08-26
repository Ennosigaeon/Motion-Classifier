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
		math::Vector *input = NULL;
		int size;

		double epsilon;
		double threshold;

		//Used for multithreading calculation
		void calc(int start, int end, double h, std::vector<math::Vector*> *result);

	public:

		/**
		  * MeanShift offers methods to calculate the Mean Shift algorithm for a array
		  * of math::Vectors. It is possible to apply filtering to the input data. Two
		  * parameters are necessary for creation: a kernel and a vector space. The
		  * kernel can be selected from math::KernelType. The vector space defines how
		  * the distances between math::Vectors are calculated. Furthermore, two 
		  * parameters are optional: epsilon describes the minimum lenght of the Mean
		  * Shift vector, threshold defines a barrier for math::Vector. Every vector with
		  * a z value smaller then z is ignored in the Mean Shift procedure.
		  */
		MeanShift(math::KernelType kernel, double epsilon = 0.05, double threshold = 30);

		~MeanShift();

		/**
		  * Sets the data points for the Mean Shift procedure. Use
		  * MeanShift::calculate(double) to calculate the clusters.
		  */
		void setDataPoints(math::Vector *input, int size);

		/**
		  * Performs the Mean Shift algorithm on the given data points (see setDataPoints).
		  * Each math::Vector is assigned to a cluster (group). In addition all center
		  * of clusters are returned. The index in the returned list corresponds to
		  * the cluster number.
		  * h:	bandwidth
		  */
		std::vector<math::Vector*>* calculate(double h);
	};
}

#endif