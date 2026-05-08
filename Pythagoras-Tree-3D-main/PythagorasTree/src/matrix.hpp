#pragma once
#include <vector>
#include <iostream>

#define matrixSize 4

/** Struct Matrix. 
 * Defines a matrix with 16 values
 */
struct Matrix {
	int rows;
	int cols;
	double matrix[16];
};

// Getter for any value [i, j] in a matrix
const double getMatrElem(Matrix, int, int);

// matrix multiplication
Matrix multiplicate(const Matrix&, const Matrix&);

// print out matrix to std::cout
void printMatrix(Matrix);