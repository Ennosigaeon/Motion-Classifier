#include <sstream>
#include "../h/Exception.h"
#include "../h/Sample.h"

Sample::Sample(int nrRows, int nrColumns, long nr) {
	Sample::nrRows = nrRows;
	Sample::nrColumns = nrColumns;
	Sample::nr = nr;
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

long Sample::getNumber() const {
	return nr;
}

std::ostream& operator<<(std::ostream& stream, const Sample& sample) {
	for (std::vector<math::Vector>::const_iterator it = sample.entries.begin(); it != sample.entries.end(); it++)
		stream << it->getZ() << " ";
	stream << std::endl;
	return stream;
}

std::istream& operator>> (std::istream& stream, Sample& sample) {
	std::string line;
	std::getline(stream, line);
	std::istringstream iss(line);
	std::vector<double> tokens{ std::istream_iterator < double > {iss}, std::istream_iterator < double > {} };

	if (tokens.size() != sample.getNrColumns() * sample.getNrRows())
		throw Exception::END_OF_FILE;

	int x = 0, y = 0;
	for (std::vector<double>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		math::Vector vec(x, y, *it);
		sample.addEntry(vec);
		y++;
		if (y == sample.getNrRows()) {
			y = 0;
			x++;
		}
	}
	return stream;
}