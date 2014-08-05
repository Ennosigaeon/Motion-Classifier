#include <cmath>
#include <sstream>
#include "../h/Vector.h"

using namespace motion_classifier;

math::Space* math::Vector::space = new math::Space;

math::Vector::Vector(double x, double y, double z, int group) {
	values[0] = x;
	values[1] = y;
	values[2] = z;
	Vector::group = group;
}

math::Vector::Vector(double *values, int group) : Vector(values[0], values[1], values[2], group) {
	delete[] values;
}

math::Vector::Vector(const math::Vector &vector) {
	//Due to better/faster runtime no loop is used
	values[0] = vector.values[0];
	values[1] = vector.values[1];
	values[2] = vector.values[2];
	group = vector.group;
}

math::Vector math::Vector::getVector(const Angle& angle) {
	switch (angle) {
	case DEGREE_0:
		return math::Vector(1, 0, 0);
	case DEGREE_30:
		return math::Vector(sqrt(3) / 2, 0.5, 0);
	case DEGREE_60:
		return math::Vector(0.5, sqrt(3) / 2, 0);
	case DEGREE_90:
		return math::Vector(0, 1, 0);
	case DEGREE_120:
		return math::Vector(-0.5, sqrt(3) / 2, 0);
	case DEGREE_150:
		return math::Vector(-sqrt(3) / 2, 0.5, 0);
	default:
		return math::Vector();
	}
}

void math::Vector::setSpace(math::Space *space) {
	if (space != NULL)
		delete space;
	Vector::space = space;
}

double math::Vector::get(const int index) const {
	return values[index];
}

void math::Vector::set(const int index, const double value) {
	values[index] = value;
}

unsigned int math::Vector::getDimensions() const {
	return DIMENSIONS;
}

double math::Vector::getDistance(const math::Vector &vector) const {
	double d = 0;
	d += (values[0] - vector.values[0]) * (values[0] - vector.values[0]);
	d += (values[1] - vector.values[1]) * (values[1] - vector.values[1]);
	//TODO: THIS IS CHANGED
	d += (values[2] - vector.values[2]) * (values[2] - vector.values[2]);// / 100;
	return sqrt(d);
}

void math::Vector::setLength(const double length, const int nrDimensions) {
	double l = length / getLength(nrDimensions);
	for (int i = 0; i < nrDimensions; ++i)
		values[i] = values[i] * l;
}

double math::Vector::getLength(const int nrDimensions) const {
	double value = 0;
	for (int i = 0; i < nrDimensions; ++i)
		value += values[i] * values[i];
	return sqrt(value);
}

int math::Vector::getGroup() const {
	return group;
}

void math::Vector::setGroup(int group) {
	Vector::group = group;
}

math::Vector math::Vector::operator+(const math::Vector& vector) const {
	math::Vector vec;
	for (int i = 0; i < DIMENSIONS; ++i)
		vec.values[i] = values[i] + vector.values[i];
	return vec;
	//return math::Vector(space->addition(values, vector.values));
}

math::Vector math::Vector::operator-(const math::Vector& vector) const {
	math::Vector vec;
	for (int i = 0; i < DIMENSIONS; ++i)
		vec.values[i] = values[i] - vector.values[i];
	return vec;
	//return math::Vector(space->subtraction(values, vector.values));
}

math::Vector math::Vector::operator*(const double value) const {
	math::Vector vec;
	for (int i = 0; i < DIMENSIONS; ++i)
		vec.values[i] = values[i] * value;
	return vec;
	//return math::Vector(space->multiplication(values, value));
}

math::Vector math::Vector::operator/(const double value) const {
	math::Vector vec;
	for (int i = 0; i < DIMENSIONS; ++i)
		vec.values[i] = values[i] / value;
	return vec;
	//return math::Vector(space->division(values, value));
}

math::Vector& math::Vector::operator+=(const math::Vector& vector) {
	for (int i = 0; i < DIMENSIONS; ++i)
		values[i] += vector.values[i];
	//*this = math::Vector(space->addition(values, vector.values));
	return *this;
}

math::Vector& math::Vector::operator/=(const double value) {
	for (int i = 0; i < DIMENSIONS; ++i)
		values[i] /= value;
	return *this;
}

math::Vector& math::Vector::operator=(const math::Vector& vector) {
	for (unsigned int i = 0; i < DIMENSIONS; ++i)
		values[i] = vector.values[i];
	group = vector.getGroup();
	return *this;
}

bool math::Vector::operator==(const math::Vector& vector) {
	for (unsigned int i = 0; i < DIMENSIONS; ++i)
		if (values[i] != vector.values[i])
			return false;
	return true;
}

std::ostream& math::operator<<(std::ostream& stream, const math::Vector& vector) {
	stream << vector.values[0]<< " " << vector.values[1] << " " << vector.values[2];
	return stream;
}

std::istream& math::operator>>(std::istream& stream, math::Vector& vector) {
	std::string line;
	std::getline(stream, line);
	if (line.empty())
		for (int i = 0; i < DIMENSIONS; ++i)
			vector.set(i, NAN);
	else {
		std::istringstream iss(line);
		for (int i = 0; i < DIMENSIONS; ++i) {
			double d;
			iss >> d;
			vector.set(i, d);
		}
	}

	return stream;
}