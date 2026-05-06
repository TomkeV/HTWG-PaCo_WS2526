#pragma once
// Collection of further structs and functions needed for renderer
#include <cmath>
#include <vector>
#include <optional>
#include "vector3D.hpp"

// -------------------------------- Forward declaration of structs --------------------------------
struct Mat4;
struct Camera;
struct ScreenPoint;
struct RenderObject;

// -------------------------------- Prototypes of declaration of inline math helpers --------------------------------
inline Mat4 multiply(const Mat4& a, const Mat4& b);
inline Vector3D transformPoint(const Mat4& m, const Vector3D& v, float& wOut);
inline std::optional<ScreenPoint> projectPoint(
    const Vector3D& worldPoint,
    const Camera& cam,
    int screenWidth,
    int screenHeight
);


// -------------------------------- Struct definitions --------------------------------
/** Struct RenderObject.
 *  Describes a 3D object with vertices and edges
 *  and defines the rgb-color the obeject should be drawn with. 
 */
struct RenderObject {
    std::vector<Vector3D> verts;            // List of 3D verts
    std::vector<std::pair<int, int>> edges; // List of edges as pairs of vertex indices
    Vector3D color;                         // (r, g, b) color of object
};

/** Struct Mat4.
 *  Defines a 4x4 matrix (column-major).
 */
struct Mat4 {
    float m[16];    // Float-array which represents the 16 matrix elements (ordered column-major).

    /* identity() returns an identity matrix.
     */
    static Mat4 identity() {
        Mat4 r{};
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }

    /* perspective() creates a perspective projection matrix.
     * @param fovYRadians Vertical field of view in radians.
     * @param aspect Aspect ratio (width/height).
     * @param zNear Near clipping plane distance.
     * @param zFar Far clipping plane distance.
     * @return Perspective projection matrix.
     */
    static Mat4 perspective(float fovYRadians, float aspect, float zNear, float zFar) {
        Mat4 r{};
        float f = 1.0f / std::tan(fovYRadians / 2.0f);
        r.m[0] = f / aspect;
        r.m[5] = f;
        r.m[10] = (zFar + zNear) / (zNear - zFar);
        r.m[11] = -1.0f;
        r.m[14] = (2.0f * zFar * zNear) / (zNear - zFar);
        return r;
    }

    /* lookAt() creates a view matrix eye -> target with given up-vector
     * @param eye Camera position in world space.
     * @param target Point the camera is looking at.
     * @param up Up direction vector.
     * @return View matrix.
     */
    static Mat4 lookAt(const Vector3D& eye, const Vector3D& target, const Vector3D& up) {
       
        Vector3D zaxis = normalize(subtractVector3d(eye, target)); // forward
        Vector3D xaxis = normalize(cross(up, zaxis));               // right
        Vector3D yaxis = cross(zaxis, xaxis);                       // up'

        Mat4 r = Mat4::identity();
        r.m[0] = xaxis.x_; r.m[4] = xaxis.y_; r.m[8] = xaxis.z_;
        r.m[1] = yaxis.x_; r.m[5] = yaxis.y_; r.m[9] = yaxis.z_;
        r.m[2] = zaxis.x_; r.m[6] = zaxis.y_; r.m[10] = zaxis.z_;

        r.m[12] = - scalarProduct(xaxis, eye);
        r.m[13] = - scalarProduct(yaxis, eye);
        r.m[14] = -scalarProduct(zaxis, eye);

        return r;
    }
};

/** Struct Camera. 
 * Contains parameters for point of view and projection.
 */
struct Camera {
    Vector3D position;
    Vector3D target;
    Vector3D up;

    // Field of view in radians: 
    float fovYRadians;
    float aspect;
    float zNear;
    float zFar;

    /* viewMatrix() computes the view matrix for the given position, target and up.
     * @return view matrix
     */
    Mat4 viewMatrix() const {
        return Mat4::lookAt(position, target, up);
    }

    /* projMatrix() computes the projection matrix out of the view in radians.
     * @return projection matrix
     */
    Mat4 projMatrix() const {
        return Mat4::perspective(fovYRadians, aspect, zNear, zFar);
    }

    /* viewProjMatrix() combines the view and the projection.
     * @return view-projection matrix
     */
    Mat4 viewProjMatrix() const {
        return multiply(projMatrix(), viewMatrix());
    }
};

/* Struct ScreenPoint.
 * Represents a 2D Point with an additional depth information.
 */
struct ScreenPoint {
    float x;
    float y;
    float depth;
};


// --------------------------------  Inline math helpers --------------------------------
/* Matrix multiplication. 
 * Multiplicates two 4x4 matrices.
 * @return product of matrix multiplication
 */
inline Mat4 multiply(const Mat4& a, const Mat4& b) {
    Mat4 r{};
    for (int c = 0; c < 4; ++c) {
        for (int rIdx = 0; rIdx < 4; ++rIdx) {
            r.m[c * 4 + rIdx] =
                a.m[0 * 4 + rIdx] * b.m[c * 4 + 0] +
                a.m[1 * 4 + rIdx] * b.m[c * 4 + 1] +
                a.m[2 * 4 + rIdx] * b.m[c * 4 + 2] +
                a.m[3 * 4 + rIdx] * b.m[c * 4 + 3];
        }
    }
    return r;
}

/* Point transformation.
 * Transforms a 3D point with a given transformation matrix.
 * @param m The transformation matrix as Mat4
 * @param v The 3D point to transform as Vector 3D
 * @param wOut Output parameter to receive the homogeneous w coordinate.
 * @return transformed 3D point as Vector 3D
  */
inline Vector3D transformPoint(const Mat4& m, const Vector3D& v, float& wOut) {
    float x = v.x_, y = v.y_, z = v.z_;
    float rx = m.m[0] * x + m.m[4] * y + m.m[8] * z + m.m[12];
    float ry = m.m[1] * x + m.m[5] * y + m.m[9] * z + m.m[13];
    float rz = m.m[2] * x + m.m[6] * y + m.m[10] * z + m.m[14];
    float rw = m.m[3] * x + m.m[7] * y + m.m[11] * z + m.m[15];
    wOut = rw;
    return { rx, ry, rz };
}

/* Projection of 3D point to 2D screen with view-projection matrix from camera.
 * @param worldPoint The 3D point (Vector3D) in world space.
 * @param cam The camera used for projection.
 * @param screenWidth width of the screen in pixels.
 * @param screenHeight height of the screen in pixels.
 * @return Optional<ScreenPoint> containing 2D coordinates and depth if the point is visible; std::nullopt otherwise.
 */
inline std::optional<ScreenPoint> projectPoint(
    const Vector3D& worldPoint,
    const Camera& cam,
    int screenWidth,
    int screenHeight
) {
    Mat4 vp = cam.viewProjMatrix();
    float w;
    Vector3D clip = transformPoint(vp, worldPoint, w);

    // check if point is visible
    if (w == 0.0f) return std::nullopt;

    float ndcX = clip.x_ / w;
    float ndcY = clip.y_ / w;
    float ndcZ = clip.z_ / w;

    if (ndcX < -1.0f || ndcX > 1.0f ||
        ndcY < -1.0f || ndcY > 1.0f ||
        ndcZ < -1.0f || ndcZ > 1.0f) {
        return std::nullopt; 
    }

    // project visible point to screen
    float sx = (ndcX + 1.0f) * 0.5f * screenWidth;
    float sy = (1.0f - ndcY) * 0.5f * screenHeight;

    return ScreenPoint{ sx, sy, ndcZ };
}