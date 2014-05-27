#ifndef SAMPLE_H
#define SAMPLE_H

#include <vector>
#include "Vector.h"

/**
  * A Sample represent all measured EMG signals for a specific timestamp.
  * The sensors are in an array with a fixed number of rows and columns.
  */
class Sample {
private:
	math::Vector *entries;
	int nrRows;
	int nrColumns;
	long nr;

public:

	/**
	  * Creates a new Sample with the given number of rows and columns.
	  * The Sample is empty. The parameter nr should be a incrementing
	  * number. It is like an ID for the Sample.	
	  */
	Sample(int nrRows = 0, int nrColumns = 0, long nr = -1);

	~Sample();

	/**
	  * Returns the number of rows in this Sample.
	  */
	int getNrRows() const;

	/**
	  * Returns the number of columns in this Sample.
	  */
	int getNrColumns() const;

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
  */
std::istream& operator>>(std::istream& stream, Sample& sample);

#endif