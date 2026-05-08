#include "matrix.hpp"
#include <iostream>

// -------------------------------- Private functions --------------------------------
/* PRIVATE
 * Getter for one row out of a matrix.
 * @param m matrix that contains row
 * @param r number of row wanted
 * @param row output param for returning chosen row
 * @return row r out of matrix m 
 */
double* getRow(struct Matrix m, int r, double* row) {
	for (int i = 0; i < m.cols; ++i) {
		row[i] = getMatrElem(m, r, i);
	}
	return row;
}


/* PRIVATE
 * Computes the scalar product out of two vectors
 * to compute the new value of an element for matrix multiplication.
 * @return v1[0] * v2[0] + v1[1] * v2[1] + ...
 */
double scalar_product(double* v1, double* v2) {
	double res = 0;
	for (int i = 0; i < matrixSize; ++i) {
		res += v1[i] * v2[i];
	}
	return res;
}


// -------------------------------- Functions --------------------------------
/* Getter.
 * Read-only access to specified element of matrix through its index (row, col).
 * @param matrix matrix containing the value wanted
 * @param r, c index (r, c) of the element wanted
 * @return matrix(r, c) as double
 */
const double getMatrElem(Matrix matrix, int r, int c) {
	return matrix.matrix[r * matrix.cols + c];
}


/* Matrix multiplication.
 * Multiplicates two matrices.
 * @param m1, m2 matrices to be multiplicated
 * @return new matrix = m1 * m2 or empty matrix with rows = cols = -1 if dimensions are wrong 
 */
Matrix multiplicate(const Matrix& m1, const Matrix& m2) {
	// Check if dimensions fit
	if (m1.cols != m2.rows) {
		struct Matrix res_matrix;
		res_matrix.rows = -1;
		res_matrix.cols = -1;
		return res_matrix;
	}

	// switch rows and cols of second matrix for easier access while multiplication
	double m2_as_cols_values[16];
	for (int col = 0; col < m2.cols; ++col) {
		for (int row = 0; row < m2.rows; ++row) {
			double value = getMatrElem(m2, row, col);
			m2_as_cols_values[col * m2.rows + row] = value;
		}
	}

	// create transformed matrix from m2_as_cols_values
	struct Matrix m2_as_cols = { m2.cols, m2.rows };
	m2_as_cols.matrix[0] = m2_as_cols_values[0];
	m2_as_cols.matrix[1] = m2_as_cols_values[1];
	m2_as_cols.matrix[2] = m2_as_cols_values[2];
	m2_as_cols.matrix[3] = m2_as_cols_values[3];

	// multiplicate
	double res_matrix_rows[4];
	for (int i = 0; i < m1.rows; ++i) {
		for (int j = 0; j < m2_as_cols.rows; ++j) {
			double row1[4];
			double row2[4];

			getRow(m1, i, row1);
			getRow(m2_as_cols, j, row2);

			double scalar_result = scalar_product(row1, row2);
			res_matrix_rows[i * m2.cols + j] = scalar_result;
		}
	}

	// create multiplicated matrix from computed rows
	struct Matrix res_matrix = { m1.rows, m2.cols};
	res_matrix.matrix[0] = res_matrix_rows[0];
	res_matrix.matrix[1] = res_matrix_rows[1];
	res_matrix.matrix[2] = res_matrix_rows[2];
	res_matrix.matrix[3] = res_matrix_rows[3];

	return res_matrix;
}

/* Output to std::cout.
 * @param m matric to be printed.
 */
void printMatrix(Matrix m) {
	for (int i = 0; i < m.rows; ++i) {
		for (int j = 0; j < m.cols; ++j) {
			std::cout << getMatrElem(m,i,j) << " ";
		}
		std::cout << std::endl;
	}
}