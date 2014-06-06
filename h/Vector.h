#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include "Angle.h"

namespace math {
	/**
	  * This class represents a threedimensional vector. It is
	  * possible to assign the Vector to a certain group (represented
	  * by an Integer).
	  */
	class Vector {
	private:
		int group = -1;
		double values[3];

	public:

		/**
		  * Creates a new Vector at the given point in a three dimensional
		  * space. The default point is (0, 0, 0).
		  */
		Vector(double x = 0, double y = 0, double z = 0);

		/**
		  * Returns a precalculated vector with length 1 and the given angle
		  * relative to the x-axis. The Vector has a z-value of 0.
		  */
		static math::Vector getVector(const math::Angle& angle);

		/**
		  * Calculates the square distance between the two given Vectors. If you want to you
		  * can limit the number of dimensions that are considured for the distance calculation.
		  * The default number of dimensions is three.
		  */
		//TODO: unused
		static double distance(const Vector& vector1, const Vector& vector2, int nrDimensions = 3);

		/**
		  * Returns the stored value in the given dimension.
		  */
		double get(const int index) const;

		/**
		  * Sets the x-value to the given value.
		  */
		inline void setX(const double x);

		/**
		  * Sets the y-value to the given value.
		  */
		inline void setY(const double y);

		/**
		  * Sets the z-value to the given value.
		  */
		inline void setZ(const double z);

		/**
		  * Returns the x-value.
		  */
		inline double getX() const;

		/**
		  * Returns the y-value.
		  */
		inline double getY() const;

		/**
		  * Returns the z-value.
		  */
		inline double getZ() const;

		/**
		  * Changes the Vector so, that its length is equal to the given lenght. The values in
		  * all dimensions will be changed.
		  */
		void setLength(const double length, const int nrDimensions = 3);

		/**
		  * Calculates the length of the given Vector.
		  */
		double getLength(const int nrDimensions = 3) const;

		/**
		  * Assigns this Vector to the given group number.
		  */
		void setGroup(int group);

		/**
		  * Returns the group number of this Vector.
		  */
		int getGroup() const;

		//operators
		math::Vector operator+(const math::Vector& vector);
		friend std::ostream& operator<<(std::ostream & stream, const math::Vector & vector);
	};

	std::istream& operator>>(std::istream& stream, math::Vector& vector);
}

#endif