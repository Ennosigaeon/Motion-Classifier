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
	std::vector<math::Vector> entries;
	int nrRows;
	int nrColumns;

public:

	/**
	  * Creates a new Sample with the given number of rows and columns.
	  * The Sample is empty.
	  */
	Sample(int nrRows = 0, int nrColumns = 0);

	/**
	  * Returns the number of rows in this Sample.
	  */
	int getNrRows() const;

	/**
	  * Returns the number of columns in this Sample.
	  */
	int getNrColumns() const;

	/**
	  * Returns all entries stored in this Sample.
	  */
	std::vector<math::Vector> getEntries() const;

	/**
	  * Adds a new Entry to this Sample.
	  */
	void addEntry(const math::Vector& entry);

	//operator
	friend std::ostream& operator<<(std::ostream & stream, const Sample & sample);
};

#endif