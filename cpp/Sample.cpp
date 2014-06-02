#include <sstream>
#include "../h/Exception.h"
#include "../h/Sample.h"

Sample::Sample(int nrRows, int nrColumns, long nr) : nrRows(nrRows), nrColumns(nrColumns), nr(nr) {
	entries = new math::Vector[nrRows * nrColumns];
}

Sample::~Sample() {
	delete[] entries;
}

math::Vector* Sample::getEntries() {
	return entries;
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
	int size = sample.getNrColumns() * sample.getNrRows();
	math::Vector *entries = sample.entries;
	for (int i = 0; i < size; ++i)
		stream << entries[i].getZ() << " ";
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

	int x = 0, y = 0, i = 0;
	math::Vector *entries = sample.getEntries();
	for (std::vector<double>::iterator it = tokens.begin(); it != tokens.end(); ++it, ++i) {
		math::Vector vec(x, y, *it);
		entries[i] = vec;
		++y;
		if (y == sample.getNrRows()) {
			y = 0;
			++x;
		}
	}

	return stream;
}