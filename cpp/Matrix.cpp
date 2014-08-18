#include "../h/Matrix.h"

using namespace motion_classifier;

math::Matrix::Matrix(int m, int n, int p) {
	Matrix::M = m;
	Matrix::N = n;
	Matrix::P = p,
	values = new double**[Matrix::M];
	for (int i = 0; i < Matrix::M; ++i) {
		values[i] = new double*[Matrix::N];
		for (int j = 0; j < Matrix::N; ++j)
			values[i][j] = new double[Matrix::P];
	}

	for (int i = 0; i < Matrix::M; ++i)
		for (int j = 0; j < Matrix::N; ++j)
			for (int k = 0; k < Matrix::P; ++k)
				values[i][j][k] = 0;

	math::Space *space = Vector::getSpace();
	int sizeX = space->getMax(math::Dimension::X) - space->getMin(math::Dimension::X);
	int sizeY = space->getMax(math::Dimension::Y) - space->getMin(math::Dimension::Y);
	int sizeZ = space->getMax(math::Dimension::Z) - space->getMin(math::Dimension::Z);
	bucketSize[math::Dimension::X] = sizeX / N;
	bucketSize[math::Dimension::Y] = sizeY / M;
	bucketSize[math::Dimension::Z] = sizeZ / P;
}

math::Matrix::~Matrix() {
	for (int i = 0; i < Matrix::M; ++i){
		for (int j = 0; j < Matrix::N; ++j)
			delete[] values[i][j];
		delete[] values[i];
	}
	delete[] values;
}

int math::Matrix::getM() const {
	return M;
}

int math::Matrix::getN() const {
	return N;
}

int math::Matrix::getP() const {
	return P;
}

double math::Matrix::get(int row, int column, int dimension) const {
	return values[row][column][dimension];
}

void math::Matrix::set(int row, int column, int dimension, double value) {
	values[row][column][dimension] = value;
}

void math::Matrix::setBucketSize(math::Dimension dimension, double value) {
	bucketSize[dimension] = value;
}

void math::Matrix::normalize() {
	double sum = 0;
	for (int i = 0; i < Matrix::M; ++i)
		for (int j = 0; j < Matrix::N; ++j)
			for (int k = 0; k < Matrix::P; ++k)
				sum += values[i][j][k];

	if (sum == 0)
		return;

	for (int i = 0; i < Matrix::M; ++i)
		for (int j = 0; j < Matrix::N; ++j)
			for (int k = 0; k < Matrix::P; ++k)
				values[i][j][k] /= sum;
}

double math::Matrix::getDistance(Matrix &matrix) const {
	if (matrix.N != N || matrix.M != M || matrix.P != P)
		throw std::range_error("Dimensions of Matrixes does not match");

	double sum = 0;

	for (int i = 0; i < Matrix::M; ++i)
		for (int j = 0; j < Matrix::N; ++j)
			for (int k = 0; k < Matrix::P; ++k)
				sum += (values[i][j][k] - matrix.values[i][j][k]) * (values[i][j][k] - matrix.values[i][j][k]);
	return sqrt(sum);
}

void math::Matrix::assignToBucket(math::Vector &vector) {
	math::Space *space = vector.getSpace();

	int column = std::floor(vector.get(math::Dimension::X) / bucketSize[math::Dimension::X]);
	int row = std::floor(vector.get(math::Dimension::Y) / bucketSize[math::Dimension::Y]);
	int dimension = std::floor(vector.get(math::Dimension::Z) / bucketSize[math::Dimension::Z]);

	//TODO: this checks are only corrent when bucketsize == 1
	if (column == space->getMax(math::Dimension::X))
		column--;
	if (row == space->getMax(math::Dimension::Y))
		row--;
	if (dimension == space->getMax(math::Dimension::Z))
		dimension--;
	if (dimension == 1) {
		int a = 0;
	}

	values[row][column][dimension]++;
}

math::Matrix* math::Matrix::getTransformationMatrix(math::Matrix &matrix) const {
	return new math::Matrix(0, 0, 0);
}

std::ostream& math::operator<<(std::ostream & stream, const math::Matrix & matrix) {
	for (int k = 0; k < matrix.getP(); ++k) {
		for (int i = matrix.getM() - 1; i >= 0; --i) {
			for (int j = 0; j < matrix.getN(); ++j)
				stream << matrix.get(i, j, k) << "\t";
			stream << std::endl;
		}
		stream << "-----------" << std::endl;
	}
	return stream;
}
