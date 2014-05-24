#include "../h/Sample.h"

Sample::Sample(int nrRows, int nrColumns) {
	Sample::nrRows = nrRows;
	Sample::nrColumns = nrColumns;
	entries.reserve(nrRows * nrColumns);
}

void Sample::addEntry(const math::Vector& entry) {
	entries.push_back(entry);
}

std::vector<math::Vector>* Sample::getEntries() {
	return &entries;
}

int Sample::getNrRows() const {
	return nrRows;
}

int Sample::getNrColumns() const {
	return nrColumns;
}

void Sample::print(std::ostream& stream) const{
	for (std::vector<math::Vector>::const_iterator it = entries.begin(); it != entries.end(); it++)
		stream << it->getX() << "\t" << it->getY() << "\t" << it->getZ() << std::endl;
}