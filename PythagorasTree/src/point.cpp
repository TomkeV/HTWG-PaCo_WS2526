#define _USE_MATH_DEFINES // needed for use of pi (M_PI) from cmath

#include "point.hpp"
#include "vector3D.hpp"
#include "matrix.hpp"
#include <cmath>
#include <iostream>
#include <vector>

// -------------------------------- Functions --------------------------------
/* Transforms a Point in 3D space by a given angle, axis and vector.
 * Angle and axis are needed for rotation, vector delays point.
 * @param point Point to be transformed
 * @param alpha Angle for rotation
 * @param axis Axis for rotation
 * @param v Vector for delaying the point
 * @return transformed point
 */
Point transform3D(Point point, double alpha, char axis, double* v) {
	// Convert 3D point in homogenous matrix 
	struct Matrix pointHomogenous = { 4, 1 };
	pointHomogenous.matrix[0] = point.x;
	pointHomogenous.matrix[1] = point.y;
	pointHomogenous.matrix[2] = point.z;
	pointHomogenous.matrix[3] = 1;

	// select correct rotational matrix belongig to axis and angle delaying by v
	struct Matrix rotation = { 4,4 };
	float sin_value = sin(alpha / 180 * M_PI);
	float cos_value = cos(alpha / 180 * M_PI);
	if (axis == 'x') { // x-axis
		rotation.matrix[0] = 1;  rotation.matrix[1] = 0;         rotation.matrix[2] = 0;          rotation.matrix[3] = v[0];
		rotation.matrix[4] = 0;  rotation.matrix[5] = cos_value; rotation.matrix[6] = -sin_value; rotation.matrix[7] = v[1];
		rotation.matrix[8] = 0;  rotation.matrix[9] = sin_value; rotation.matrix[10] = cos_value; rotation.matrix[11] = v[2];
		rotation.matrix[12] = 0; rotation.matrix[13] = 0;        rotation.matrix[14] = 0;         rotation.matrix[15] = 1;
	}
	else if (axis == 'y') { // y-axis
		rotation.matrix[0] = cos_value;  rotation.matrix[1] = 0;  rotation.matrix[2] = sin_value;  rotation.matrix[3] = v[0];
		rotation.matrix[4] = 0;          rotation.matrix[5] = 1;  rotation.matrix[6] = 0;          rotation.matrix[7] = v[1];
		rotation.matrix[8] = -sin_value; rotation.matrix[9] = 0;  rotation.matrix[10] = cos_value; rotation.matrix[11] = v[2];
		rotation.matrix[12] = 0;         rotation.matrix[13] = 0; rotation.matrix[14] = 0;         rotation.matrix[15] = 1;
	}
	else { // z-axis
		rotation.matrix[0] = cos_value; rotation.matrix[1] = -sin_value; rotation.matrix[2] = 0;  rotation.matrix[3] = v[0];
		rotation.matrix[4] = sin_value; rotation.matrix[5] = cos_value;  rotation.matrix[6] = 0;  rotation.matrix[7] = v[1];
		rotation.matrix[8] = 0;         rotation.matrix[9] = 0;          rotation.matrix[10] = 1; rotation.matrix[11] = v[2];
		rotation.matrix[12] = 0;        rotation.matrix[13] = 0;         rotation.matrix[14] = 0; rotation.matrix[15] = 1;
	}

	// rotate point
	struct Matrix res = multiplicate(rotation, pointHomogenous);

	// convert result back into point instead of matrix
	struct Point res_point = { getMatrElem(res,0,0), getMatrElem(res,1,0), getMatrElem(res,2,0) };

	return res_point;
}

/* Convert point to Vector3D.
 */
Vector3D toVector3D(Point p) {
	return { p.x, p.y, p.z };
}


// NEU NEU
double* toArray(Point p) {
	double* array = (double*)malloc(3 * sizeof(double));
	array[0] = p.x;
	array[1] = p.y;
	array[2] = p.z;
	//std::cout << "array: " << array[0] << array[1] << array[2] << std::endl;
	return array;
}