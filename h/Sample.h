#ifndef SAMPLE_H
#define SAMPLE_H

#include <vector>
#include "Vector.h"

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

	~Sample();

	/**
	  * Returns the number of rows in this Sample.
	  */
	inline int getNrRows() const;

	/**
	  * Returns the number of columns in this Sample.
	  */
	inline int getNrColumns() const;

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

/**
  * Loads a sample from the given stream. All values for the sample has to be in
  * a single line. The values have to be seperated by a blank. The values are
  * interpreted as columns.
  * Throws	std::out_of_range
  */
std::istream& operator>>(std::istream& stream, Sample& sample);

#endif