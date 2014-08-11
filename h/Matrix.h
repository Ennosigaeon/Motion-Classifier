#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>
#include "Vector.h"

namespace motion_classifier {
	namespace math {

		class Matrix {
		private:
			double ***values;

			//Rows
			int M;

			//Columns
			int N;

			//Dimensions
			int P;

			double bucketSize[DIMENSIONS];

		public:

			Matrix(int m, int n, int p);
			~Matrix();

			//Returns number of rows
			int getM() const;

			//Returns number of columns
			int getN() const;

			int getP() const;

			double get(int row, int column, int p) const;

			void set(int row, int column, int p, double value);

			void setBucketSize(math::Dimension dimension, double value);

			void normalize();

			double getDistance(Matrix &matrix) const;

			void assignToBucket(math::Vector &vector);

			Matrix* getTransformationMatrix(Matrix &matrix) const;

			friend std::ostream& operator<<(std::ostream & stream, const math::Matrix & matrix);
		};

	}
}

#endif