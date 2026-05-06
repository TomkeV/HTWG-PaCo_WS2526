#include "cuboid.hpp"
#include <cstring>

// -------------------------------- "Constructors" --------------------------------
/* "Constructor". 
 * Creates a new cuboid with corner A in (0|0|0) out of its height, width and depth.
 */
Cuboid createCuboidFromSidelength(double height, double width, double depth) {
    static const int xedges[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3 };
    static const int yedges[12] = { 1, 2, 3, 0, 5, 6, 7, 4, 4, 5, 6, 7 };

    Cuboid c = {
        .points = {
            { 0,     0,     0      },
            { width, 0,     0      },
            { width, depth, 0      },
            { 0,     depth, 0      },
            { 0,     0,     height },
            { width, 0,     height },
            { width, depth, height },
            { 0,     depth, height }
        },
        .angle = 0
    };
    std::memcpy(c.xedges, xedges, sizeof(xedges));
    std::memcpy(c.yedges, yedges, sizeof(yedges));
    return c;
}

// -------------------------------- Functions --------------------------------
/* Computes the sidelength of this cuboid.
 * @returns {a (width), b (depth), h (height)}
 */
double* getSidelength(Cuboid cuboid) {
    double* sidelength = (double*) malloc(3 * sizeof(double));
    sidelength[0] = getLengthOf3DVector(createVector3dFromPoints(cuboid.points[1], cuboid.points[2])); // AB
    sidelength[1] = getLengthOf3DVector(createVector3dFromPoints(cuboid.points[2], cuboid.points[3])); // BC
    sidelength[2] = getLengthOf3DVector(createVector3dFromPoints(cuboid.points[1], cuboid.points[5])); // AE
    return sidelength;
}

/* Turns the cuboid by the given angle around the given axis 
 * and moves it from (0|0|0) to the place the vector shows
 * @returns new cuboid
 */
Cuboid transform(Cuboid cuboid, double alpha, char axis, double* v) {
    struct Cuboid new_cuboid;
    struct Point* old_points = cuboid.points;

    for (int i = 0; i < 8; i++) {
        struct Point current = old_points[i];
        new_cuboid.points[i] = transform3D(current, alpha, axis, v); // transforms each point to its new position
    }

    static unsigned const int xedges[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3 };
    static unsigned const int yedges[12] = { 1, 2, 3, 0, 5, 6, 7, 4, 4, 5, 6, 7 };

    std::memcpy(new_cuboid.xedges, xedges, sizeof(xedges));
    std::memcpy(new_cuboid.yedges, yedges, sizeof(yedges));
    return new_cuboid;
}
