
#ifndef SAMPLE_H
#define SAMPLE_H

#include <vector>
#include "Vector.h"

class Sample {
private:
	std::vector<math::Vector> entries;
	int nrRows;
	int nrColumns;

public:
	Sample(int nrRows = 0, int nrColumns = 0);

	int getNrRows() const;
	int getNrColumns() const;
	std::vector<math::Vector> getEntries() const;
	void addEntry(const math::Vector& entry);
	void setEntries(const std::vector<math::Vector>& entries);

	friend std::ostream& operator<<(std::ostream & stream, const Sample & sample);
};

#endif