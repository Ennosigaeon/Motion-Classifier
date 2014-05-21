#include "../h/Sample.h"

Sample::Sample(int nrRows, int nrColumns) {
	Sample::nrRows = nrRows;
	Sample::nrColumns = nrColumns;
}

void Sample::addEntry(const math::Vector& entry) {
	entries.push_back(entry);
}

std::vector<math::Vector> Sample::getEntries() const {
	return entries;
}

int Sample::getNrRows() const {
	return nrRows;
}

int Sample::getNrColumns() const {
	return nrColumns;
}

std::ostream& operator<<(std::ostream& stream, const Sample& sample) {
	std::vector<math::Vector> entries = sample.getEntries();
	for (std::vector<math::Vector>::iterator it = entries.begin(); it != entries.end(); it++)
		stream << it->getX() << "\t" << it->getY() << "\t" << it->getZ() << std::endl;
	return stream;
}