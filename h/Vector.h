#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include "MathEnums.h"
#include "Space.h"

namespace motion_classifier {
	namespace math {
		/**
		  * This class represents a threedimensional vector. It is
		  * possible to assign the Vector to a certain group (represented
		  * by an Integer).
		  */
		class Vector {
		private:
			static Space *space;
			int group;
			double values[DIMENSIONS];

		public:

			/**
			  * Creates a new Vector at the given point in a three dimensional
			  * space. The default point is (0, 0, 0). If no group is specified
			  * the vector is assigned to group -1 (no group).
			  */
			Vector(double x = 0, double y = 0, double z = 0, int group = -1);

			/**
			  * Creates a new math::Vector with the given data points. values has
			  * to be a pointer to an array with at least three elements. The array
			  * has to be allocated with new.
			  */
			Vector(double *values, int group = -1);

			/**
			  * Copies the given math::Vector to this math::Vector.
			  */
			Vector(const math::Vector &vector);

			/**
			  * Returns a precalculated vector with length 1 and the given angle
			  * relative to the x-axis. The Vector has a z-value of 0.
			  */
			static math::Vector getVector(const math::Angle& angle);

			/**
			  * Sets the given math::Space for all math::Vectors. 
			  */
			static void setSpace(math::Space *space);

			/**
			  * Returns the math::Space.
			  */
			static math::Space* getSpace();

			/**
			  * Returns the stored value in the given dimension.
			  */
			double get(const int index) const;

			/**
			  * Stores the given value in the given dimension.
			  */
			void set(const int index, const double value);

			/**
			  * Returns the length of this vector.
			  */
			inline unsigned int getDimensions() const;

			/**
			  * Calculates the distance between this math::Vector and the given one.
			  */
			double getDistance(const math::Vector &vector) const;

			/**
			  * Changes the Vector so, that its length is equal to the given lenght.
			  * The values in all dimensions will be changed.
			  */
			void setLength(const double length, const int nrDimensions = 3);

			/**
			  * Calculates the length of the given math::Vector.
			  */
			double getLength(const int nrDimensions = 3) const;

			/**
			  * Assigns this Vector to the given group number.
			  */
			void setGroup(int group);

			/**
			  * Returns the group number of this Vector.
			  */
			inline int getGroup() const;

			//operators
			math::Vector operator+(const math::Vector& vector) const;
			math::Vector operator-(const math::Vector& vector) const;
			math::Vector operator*(const double value) const;
			math::Vector operator/(const double value) const;
			math::Vector& operator+=(const math::Vector& vector);
			math::Vector& operator/=(const double value);
			math::Vector& operator=(const math::Vector& vector);
			bool operator==(const math::Vector& vector);
			bool operator!=(const math::Vector& vector);
			friend std::ostream& operator<<(std::ostream & stream, const math::Vector & vector);
		};

		std::istream& operator>>(std::istream& stream, math::Vector& vector);
	}
}

#endif