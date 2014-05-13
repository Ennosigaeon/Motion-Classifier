
#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include "Angle.h"

namespace math {

	class Vector {
	private:
		int size = 3;
		bool marked = false;
		double values[3];

	public:
		Vector(double x = 0, double y = 0, double z = 0);

		//returns a precalculated vector with lenght 1 with the given angle
		static math::Vector getVector(const math::Angle& angle);

		//calculates the distance between the two Vectors in the given number of dimensions. 
		static double distance(const Vector& vector1, const Vector& vector2, int nrDimensions);

		//stores the given value in the given dimension
		void set(const int index, const double value);

		//returns the stored value in the given dimension
		double get(const int index) const;

		void setX(const double x);

		void setY(const double y);

		void setZ(const double z);

		double getX() const;

		double getY() const;

		double getZ() const;

		//changes the x and y value, so that the lenght of this vector is equal to the given length
		void setLength(const double length);

		//returns the lenght of the vector
		double getLength() const;

		//marks the vector
		bool isMarked() const;

		//test whether this vector is marked
		void mark();

		//operators
		math::Vector operator+(const math::Vector& vector);
		bool operator<(const math::Vector& vector) const;
		friend std::ostream& operator<<(std::ostream & stream, const math::Vector & vector);
	};
}

#endif