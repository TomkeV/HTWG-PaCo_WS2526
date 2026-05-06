#pragma once
#include <vector>
#include "point.hpp"

/** Struct Cuboid.
 *	Defines a cuboid by its eight corners like this:
 *	  H -- G
 *   /    /|
 *  E -- F |
 *  |    | |
 *  | D -| C
 *  A -- B
 * Sidelengths: AB = width / BC = depth / AE = height
 */
struct Cuboid {
	Point points[8]; // coordinates of the corners
	int xedges[12];
	int yedges[12];
	double angle;
};

// Getter for all three sidelength of the cuboid
double* getSidelength(Cuboid);

// Transformation of the cuboid by rotation around given axis by angle and relocation with given vector
Cuboid transform(Cuboid, double, char, double*);

// "Constructor", creates a new cuboid from given sidelength height, width and depth
Cuboid createCuboidFromSidelength(double, double, double);
