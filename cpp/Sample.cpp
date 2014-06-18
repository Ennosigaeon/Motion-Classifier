#include <sstream>
#include "../h/AppConfig.h"
#include "../h/Sample.h"

using namespace motion_classifier;

Sample::Sample(long nr) : nr(nr) {
	AppConfig *config = AppConfig::getInstance();
	nrRows =config->getSampleRows();
	nrColumns = config->getSampleColumns();
	entries = new math::Vector[nrRows * nrColumns];
}

Sample::Sample(std::vector<short>& values, long nr) : Sample(nr) {
	if (values.size() > nrRows * nrColumns)
		throw std::out_of_range("parameter contains more values than this Sample can store");

	int x = 0, y = 0, i = 0;
	for (std::vector<short>::iterator it = values.begin(); it != values.end(); ++it, ++i) {
		math::Vector vec(x, y, *it);
		entries[i] = vec;
		++y;
		if (y == nrRows) {
			y = 0;
			++x;
		}
	}
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

std::ostream& motion_classifier::operator<<(std::ostream& stream, const Sample& sample) {
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

	int size = sample.getNrColumns() * sample.getNrRows();
	if (tokens.size() < size)
		throw std::out_of_range("line contains not enough values. Maybe reached end of file");

	int x = 0, y = 0, i = 0;
	math::Vector *entries = sample.getEntries();
	for (std::vector<double>::iterator it = tokens.begin(); it != tokens.end() && i < size; ++it, ++i) {
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