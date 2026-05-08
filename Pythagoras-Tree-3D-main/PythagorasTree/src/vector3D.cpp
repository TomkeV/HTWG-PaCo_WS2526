#define _USE_MATH_DEFINES // needed for use of pi (M_PI) from cmath

// class Vector3D
#include "vector3D.hpp"
#include <cmath>

// -------------------------------- Constructors -------------------------------- 
/* "Constructor".
 * Creates a new 3D-Vector out of the given coordinates.
 * 			/x\
 * @return | y |
 *			\z/
 */
Vector3D createVector3dFromValues(double x, double y, double z) {
	return { x, y, z };
}

/* "Constructor".
 * Creates a new instance of type Vector3D which describes the way from Point a to Point b as
 *     /b.x - a.x\
 * v = |b.y - a.y|
 *     \b.z - a.z/
 */
Vector3D createVector3dFromPoints(Point a, Point b) {
	return {
		b.x - a.x,
		b.y - a.y,
		b.z - a.z
	};
}

// -------------------------------- Functions --------------------------------
/* Computes the length of the vector as its magnitude.
 * @param v Vector to get the length of
 * @return sqrt(x^2 + y^2 + z^2)
 */
double getLengthOf3DVector(Vector3D v) {
	return sqrt(
		v.x_ * v.x_
		+ v.y_ * v.y_
		+ v.z_ * v.z_
	);
}


/* Normalizes the vector to a length of 1.
 * @param v Vector to normalize
 * @return normalized Vector with length 1
 */
Vector3D normalize(Vector3D v) {
	double normFactor = 1.0 / getLengthOf3DVector(v);
	if (normFactor == 0.0) {
		return v;
	}
	return { v.x_ * normFactor, v.y_ * normFactor, v.z_ * normFactor };
}

/* Computation of the cross product.
 * @param a, b Vectors to be crossed
 * 			/a.y_ * b.z_ - a.z_ * b.y_ \
 * @return | a.z_ * b.x_ - a.x_ * b.z_  |
 *  		\a.x_ * b.y_ - a.y_ * b.x_ /
 */
Vector3D cross(const Vector3D& a, const Vector3D& b) {
	return {
		a.y_ * b.z_ - a.z_ * b.y_,
		a.z_ * b.x_ - a.x_ * b.z_,
		a.x_ * b.y_ - a.y_ * b.x_
	};
}

/* Computation of the scalar product out of two vectors.
 * @param a, b Vectors to get the scalar product from
 * @return a(x) * b(x) + a(y) * b(y) + a(z) * b(z) as double
 */
double scalarProduct(const Vector3D& a, const Vector3D& b) {
	return a.x_ * b.x_ + a.y_ * b.y_ + a.z_ * b.z_;
}

/**
 * @brief Subtract two vectors.
 * @param o The vector to subtract.
 * @return Result of vector subtraction a - b
 */
/* Subtraction of two 3D-Vectors.
 * @param a Vector to subtract the other vector from
 * @param b Vector that gets subtracted
 * 			/ a(x) - b(x) \
 * @return |  a(y) - b(y) |
 *			\ a(z) - b(z) /
 */
Vector3D subtractVector3d(const Vector3D& a, const Vector3D& b) {
	return { a.x_ - b.x_, a.y_ - b.y_, a.z_ - b.z_ };
}
