#pragma once
#include <vector>
#include "vector3D.hpp"
#include <cmath>

// Forward declaration to use in functions
struct Vector3D;

/** Struct Point.
 * Defines a point in a 3D coordinate system by its three coordinates
 * in form P = (x|y|z).
 * 
 * Coordinate system: 
 *  z  y 
 *  | /
 *  |/
 * ------- x
 * /|
 */
struct Point {
	double x;
	double y;
	double z;
};

// 3D Transformation of point
Point transform3D(Point point, double, char, double*);

// convert point into Vector3D
Vector3D toVector3D(Point p);


// NEU NEU
double* toArray(Point p);