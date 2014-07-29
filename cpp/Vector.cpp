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
	for (unsigned int i = 0; i < DIMENSIONS; ++i)
		values[i] = vector.get(i);
	group = vector.getGroup();
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
	Vector::space = space;
}

double math::Vector::get(const int index) const {
	return values[index];
}

void math::Vector::set(const int index, const double value) {
	values[index] = value;
}

inline unsigned int math::Vector::getDimensions() const {
	return DIMENSIONS;
}

double math::Vector::getX() const {
	return values[0];
}

double math::Vector::getY() const {
	return values[1];
}

double math::Vector::getZ() const {
	return values[2];
}

void math::Vector::setX(const double value) {
	values[0] = value;
}

void math::Vector::setY(const double value) {
	values[1] = value;
}

void math::Vector::setZ(const double value) {
	values[2] = value;
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
	stream << vector.getX() << " " << vector.getY() << " " << vector.getZ();
	return stream;
}

std::istream& math::operator>>(std::istream& stream, math::Vector& vector) {
	std::string line;
	std::getline(stream, line);
	if (line.empty()) {
		vector.setX(NAN);
		vector.setY(NAN);
		vector.setZ(NAN);
	}
	else {
		std::istringstream iss(line);
		double x, y, z;
		iss >> x >> y >> z;
		vector.setX(x);
		vector.setY(y);
		vector.setZ(z);
	}

	return stream;
}