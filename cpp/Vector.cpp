
#include <cmath>
#include "../h/Vector.h"

math::Vector::Vector(double x, double y, double z) {
	values[0] = x;
	values[1] = y;
	values[2] = z;
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
		std::cerr << "This Angle is not implemented" << std::endl;
		return math::Vector();
	}
}

double math::Vector::distance(const math::Vector& vector1, const math::Vector& vector2, int nrDimensions) {
	double value = 0;
	for (int i = 0; i < nrDimensions; i++)
		value += pow(vector1.get(i) - vector2.get(i), 2);
	return sqrt(value);
}

double math::Vector::get(const int index) const {
	return values[index];
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

void math::Vector::set(const int index, const double value) {
	values[index] = value;
}

void math::Vector::setLength(const double length) {
	double l = getLength();
	for (int i = 0; i < 3; i++)
		values[i] = values[i] * length / l;
}

double math::Vector::getLength() const {
	double value = 0;
	for (int i = 0; i < 3; i++)
		value += values[i] * values[i];
	return sqrt(value);
}

bool math::Vector::isMarked() const {
	return marked;
}

void math::Vector::mark() {
	marked = true;
}

math::Vector math::Vector::operator+(const math::Vector& vector) {
	math::Vector vec;
	//the vector has 3 dimensions
	for (int i = 0; i < 3; i++)
		vec.set(i, vector.get(i) + values[i]);
	return vec;
}

bool math::Vector::operator<(const math::Vector& vector) const {
	return values[0] < vector.get(0);
}

std::ostream& math::operator<<(std::ostream& stream, const math::Vector& vector) {
	stream << "(" << vector.getX() << ", " << vector.getY() << ", " << vector.getZ() << ")";
	return stream;
}
