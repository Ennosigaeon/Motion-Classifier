#ifndef SAMPLE_H
#define SAMPLE_H

#include <vector>
#include "Vector.h"

namespace motion_classifier {
	/**
	  * A Sample represent all measured EMG signals for at a timestamp. The
	  * measured values are stored in an array with a fixed number of rows
	  * and columns. You can not change the size of the array during runtime,
	  * all Samples have to have the same size.
	  */
	class Sample {
	private:
		math::Vector *entries;
		int nrRows;
		int nrColumns;
		long nr;

	public:

		/**
		  * Creates a new Sample with the given number. The Sample is empty.
		  * The parameter nr should be a incrementing number. It is like an
		  * ID for the Sample.
		  */
		Sample(long nr = -1);

		/**
		  * Creates a new Sample with given number. The Sample is filled
		  * with the given values. The parameter nr should be a incrementing
		  * number. It is like an ID for the Sample.
		  * Throws	std::out_of_range
		  */
		Sample(std::vector<short>& values, long nr = -1);

		/**
		  * Creates a new Sample with the given number. The sample is filled
		  * with the given values. The array has to have the same size as
		  * the Sample size defined in the configuration file.
		  **/
		Sample(double* values, long nr = -1);

		Sample(const Sample &sample);

		~Sample();

		/**
		  * Returns the number of rows in this Sample.
		  */
		inline int getNrRows() const;

		/**
		  * Returns the number of columns in this Sample.
		  */
		int getNrColumns() const;

		/**
		  * Returns the total number of values in this Sample.
		  */
		inline int getSize() const;

		/**
		  * Returns the number of this Sample.
		  */
		long getNumber() const;

		/**
		  * Returns all entries stored in this Sample.
		  */
		math::Vector* getEntries();

		//operator
		friend std::ostream& operator<<(std::ostream& stream, const Sample& sample);
	};
}

/**
  * The values are all stored in one line. Start is with sensor at position (0/0)
  * corresponding to left bottom. Then the first column is filled from up to down.
  * This means the second value is at position (0/1), ... until (0/7). Then the
  * second column is filled, again from bottom to top, i.e. (1/0),....(1/7). 
  * Seperator has to be a tab or blank.
  * Throws: std::out_of_range
  */
std::istream& operator>>(std::istream& stream, motion_classifier::Sample& sample);

#endif