#include <sstream>
#include "../h/AppConfig.h"
#include "../h/Sample.h"

using namespace motion_classifier;

Sample::Sample(long nr) : nr(nr) {
	AppConfig *config = AppConfig::getInstance();
	nrRows = config->getSampleRows();
	nrColumns = config->getSampleColumns();
	entries = new math::Vector[nrRows * nrColumns];
}

Sample::Sample(std::vector<short>& values, long nr) : Sample(nr) {
	if (values.size() > nrRows * nrColumns)
		throw std::out_of_range("parameter contains more values than this Sample can store");

	int x = 0, y = 0, i = 0;
	for (const auto z : values) {
		entries[i++] = math::Vector(x, y, z);
		++y;
		if (y == nrRows) {
			y = 0;
			++x;
		}
	}
}

Sample::Sample(double* values, long nr) : Sample(nr) {
	int x = 0, y = 0;
	for (int i = 0; i < nrColumns * nrRows; i++) {
		entries[i] = math::Vector(x, y, values[i]);
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

int Sample::getSize() const {
	return nrColumns * nrRows;
}

long Sample::getNumber() const {
	return nr;
}

std::ostream& motion_classifier::operator<<(std::ostream& stream, const Sample& sample) {
	for (int i = 0; i < sample.getSize(); ++i)
		stream << sample.entries[i].get(math::Dimension::Z) << " ";
	stream << std::endl;
	return stream;
}

std::istream& operator>> (std::istream& stream, Sample& sample) {
	std::string line;
	std::getline(stream, line);
	std::istringstream iss(line);
	std::vector<double> tokens{ std::istream_iterator < double > {iss}, std::istream_iterator < double > {} };

	if (tokens.size() < sample.getSize())
		throw std::out_of_range("line contains not enough values. Maybe reached end of file");

	math::Vector *entries = sample.getEntries();
	int x = 0, y = 0, i = 0;
	for (const double z : tokens) {
		entries[i++] = math::Vector(x, y, z);
		++y;
		if (y == sample.getNrRows()) {
			y = 0;
			++x;
		}
	}

	return stream;
}