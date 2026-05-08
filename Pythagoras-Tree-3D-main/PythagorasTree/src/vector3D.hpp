#pragma once
#include "point.hpp"
#include <vector>

struct Point;

/** Struct Vector3D.
 * Defines a vector in a 3D coordinate system as
 *     /x\
 * v = |y|
 *     \z/
 * Coordinate system:
 *  z  y
 *  | /
 *  |/
 * ------- x
 * /|
 */
struct Vector3D {
	double x_;
	double y_;
	double z_;
};

// "Constructor", creates a new 3D-vector from three values
Vector3D createVector3dFromValues(double, double, double);	

// "Constructor", creates a new 3D-vector describing the way between two points
Vector3D createVector3dFromPoints(Point, Point);

// Computes the length of the 3D-Vector
double getLengthOf3DVector(Vector3D);

// Normalizes the given 3D-Vector to length = 1
Vector3D normalize(Vector3D);

// Computes the cross product out of two Vectors
Vector3D cross(const Vector3D&, const Vector3D&);

// Computation of scalar product out of two 3D-Vectors
double scalarProduct(const Vector3D&, const Vector3D&);

// Subtraction of two 3D-Vectors
Vector3D subtractVector3d(const Vector3D&, const Vector3D&);
