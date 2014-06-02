#include <cmath>
#include <sstream>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
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
		BOOST_LOG_TRIVIAL(error) << "This Angle is not implemented";
		return math::Vector();
	}
}

double math::Vector::distance(const math::Vector& vector1, const math::Vector& vector2, int nrDimensions) {
	double value = 0;
	for (int i = 0; i < nrDimensions; i++)
		value += pow(vector1.get(i) - vector2.get(i), 2);
	return value;
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

void math::Vector::setLength(const double length, const int nrDimensions) {
	double l = length / getLength(nrDimensions);
	for (int i = 0; i < nrDimensions; i++)
		values[i] = values[i] * l;
}

double math::Vector::getLength(int nrDimensions) const {
	double value = 0;
	for (int i = 0; i < nrDimensions; i++)
		value += values[i] * values[i];
	return sqrt(value);
}

int math::Vector::getGroup() const {
	return group;
}

void math::Vector::setGroup(int group) {
	Vector::group = group;
}

math::Vector math::Vector::operator+(const math::Vector& vector) {
	math::Vector vec;
	vec.setX(values[0] + vector.getX());
	vec.setY(values[1] + vector.getY());
	vec.setZ(values[2] + vector.getZ());
	return vec;
}

bool math::Vector::operator<(const math::Vector& vector) const {
	return values[0] < vector.get(0);
}

std::ostream& math::operator<<(std::ostream& stream, const math::Vector& vector) {
	stream << vector.getX() << " " << vector.getY() << " " << vector.getZ();
	return stream;
}

std::istream& math::operator>>(std::istream& stream, math::Vector& vector) {
	std::string line;
	std::getline(stream, line);
	std::istringstream iss(line);
	double x, y, z;
	iss >> x >> y >> z;
	vector.setX(x);
	vector.setY(y);
	vector.setZ(z);

	return stream;
}