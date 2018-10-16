#include <Eigen/Dense>
#include <iostream>
#include "Surfaces.h"

using Eigen::Vector3d;
using std::cout;
using std::endl;

////////////////////////////
///// Triangle class: //////
////////////////////////////
/**
 * constuct a Triangle object
 *
 * vertices: array of 3 vertices (Vector3d objects) for this triangle
 * matr:     pointer to material for this triangle
 */

Triangle::Triangle(Vector3d points[3], Material *matr)
    : Surface(matr), patch(false)
{
    for (int i=0; i<3; i++) {
        this->points[i] = points[i];
        this->norms[i] = Vector3d(0,0,0);
    }
}

/**
 * constructs a Triangle (patch) object (a triangle where the normals of the vertices are known)
 *
 * vertices: array of 3 vertices (Vector3d objects) for this triangle
 * matr:     pointer to material for this triangle
 * norms:    normals of the triangles vertices
 */
Triangle::Triangle(Vector3d points[3], Vector3d norms[3], Material *matr)
    : Surface(matr), patch(true)
{
    for (int i=0; i<3; i++) {
        this->points[i] = points[i];
        this->norms[i] = norms[i];
    }
}

/*
 * returns HitRecord with info about intersection of the given Ray and this Triangle
 * not an intersection unless t0 <= t <= t1
 * (no upper bound if t1 is -1)
 */
HitRecord Triangle::intersect(Ray ray, double d0, double d1, bool debug) const {
    // ray: e + td
    // triangle a + B(b-a) + g(c-a)
    // solve system: Ax=b
    // using Cramer's rule (see textbook p. 78)
    if (debug)
        cout << "\nintersecting " << *this << " with " << ray << endl;

    Eigen::Matrix3d A, M1, M2, M3;
    Vector3d b;
    A << points[0]-points[1], points[0]-points[2], ray.dir;

    // TODO: check matrices have determinants?
    M1 << points[0]-ray.eye, points[0]-points[2], ray.dir;
    M2 << points[0]-points[1], points[0]-ray.eye, ray.dir;
    M3 << points[0]-points[1], points[0]-points[2], points[0]-ray.eye;
    double det = A.determinant();
    double B = M1.determinant() / det;
    double g = M2.determinant() / det;
    double t = M3.determinant() / det;
    double a = 1.0 - B - g; // TODO: is this right?

    HitRecord hit(SurfaceType::TRIANGLE, -1);
    // check conditions to see if solution is valid
    if (t>0 && 0<=B && 0<=g && B+g<=1) {
        double dist = (t * ray.dir).norm();
        if (d0 <= dist && (dist <= d1 || d1 == -1)) {
            Vector3d point = ray.eye + t * ray.dir;  // intersection point
            dist = (point - ray.eye).norm();
            hit = HitRecord(SurfaceType::TRIANGLE, t, dist, point, a, B, g);
        }
    }
    if (debug)
        cout << hit << endl;
    return hit;
}

/**
 * return the surface normal of this triangle at the given point
 * the normal of a triangle is same throughout the surface
 * (unless this is a triangle patch and we're doing phong shading)
 */
Vector3d Triangle::getNormal(HitRecord hit, bool interpolate) const {
    if (patch && interpolate) {
        // interpolate normal based on normal of the vertices
        // (see p. 45 in textbook)
        return hit.a * norms[0] + hit.B * norms[1] + hit.g * norms[2];
    }
    return (points[1]-points[0]).cross(points[2]-points[1]).normalized();
}

/**
 * print out Triangle object for debugging
 */
std::ostream &operator<<(std::ostream &sout, const Triangle &tri) {
    sout << "Triangle: ";
    sout << "(" << tri.points[0][0] << "," << tri.points[0][1] << "," << tri.points[0][2] << ")" << endl;
    if (tri.patch)
        sout << "normal: (" << tri.norms[0][0] << "," << tri.norms[0][1] << "," << tri.norms[0][2] << ")" << endl;
    sout << "(" << tri.points[1][0] << "," << tri.points[1][1] << "," << tri.points[1][2] << ")" << endl;
    if (tri.patch)
        sout << "normal: (" << tri.norms[1][0] << "," << tri.norms[1][1] << "," << tri.norms[1][2] << ")" << endl;
    sout << "(" << tri.points[2][0] << "," << tri.points[2][1] << "," << tri.points[2][2] << ")" << endl;
    if (tri.patch)
        sout << "normal: (" << tri.norms[2][0] << "," << tri.norms[2][1] << "," << tri.norms[2][2] << ")" << endl;
    return sout;
}



////////////////////////////
////// Polygon class: //////
////////////////////////////
/**
 * constuct a polygon object
 * for now this class only works for convex polygons
 *
 * vertices: vector of vertices (Vector3d objects) for this polygon
 * color:    the color of this polygon (1,1,1) for white
 *           (defaults to black (0,0,0))
 */
Polygon::Polygon(Material *matr, std::vector<Vector3d> vertices, bool patch, std::vector<Vector3d> normals)
    : Surface(matr), vertices(vertices), patch(patch), normals(normals)
{
    // create triangle fan
    for (unsigned int i=2; i<vertices.size(); i++) {
        Vector3d points[3] = {vertices[0], vertices[i-1], vertices[i]};
        if (!patch) {
            triangles.push_back(Triangle(points, matr));
        }
        else {
            Vector3d norms[3] = {normals[0], norms[i-1], norms[i]};
            triangles.push_back(Triangle(points, norms, matr));
        }
    }
}

/**
 * returns distance along ray at which it intersects this polygon
 * (-1 if no intersection)
 *
 * ray:     the ray to check for intersection with
 * hither:  ignore intersections that are closer than hither (-1 to disable)
 * debug:   whether or not to print out debug info for this intersection check
 */
HitRecord Polygon::intersect(Ray ray, double d0, double d1, bool debug) const {
    HitRecord bestHit = HitRecord(SurfaceType::POLYGON, -1);

    for (unsigned int i=0; i<triangles.size(); i++) {
        // check for intersection of ray with this triangle
        HitRecord hit = triangles[i].intersect(ray, d0, d1, debug);

        // check if we should update bestHit
        if (hit.t != -1 && (bestHit.t == -1 || hit.t < bestHit.t)) {
            // we hit a triangle for the first time, or a closer triangle
            // update bestHit
            hit.sType = SurfaceType::POLYGON;
            hit.triIndex = i;
            bestHit = hit;
        }
    }
    return bestHit;
}

/**
 * returns the normal of the surface at hit.point
 * where the given hit record is known to belong to this surface
 * and hit.t != -1
 */
Vector3d Polygon::getNormal(HitRecord hit, bool interpolate) const {
    // return the .getNormal() of the triangle we intersected
    return triangles[hit.triIndex].getNormal(hit, interpolate);
}

/**
 * print out the triangle fan for this polygon
 * (for debugging)
 */
void Polygon::printTriangles() const {
    std::cout << "Triangles in this polygon:" << std::endl;
    for (unsigned int i=0; i<triangles.size(); i++) {
        std::cout << "   " << triangles[i] << std::endl;
    }
}

/**
 * print out Polygon object for debugging
 */
std::ostream& operator<<(std::ostream &sout, const Polygon &poly) {
    sout << "Polygon (" << poly.vertices.size() << " vertices):" << std::endl;
    for (unsigned int i = 0; i != poly.vertices.size(); i++) {
        sout << "   ";
        sout << "(" << poly.vertices[i][0] << "," << poly.vertices[i][1] << "," << poly.vertices[i][2] << ")" << endl;
    }
    return sout;

    sout << "Polygon (" << poly.vertices.size() << " vertices):" << std::endl;
    for (unsigned int i = 0; i != poly.vertices.size(); i++) {
        sout << "   ";
        sout << "(" << poly.vertices[i][0] << "," << poly.vertices[i][1] << "," << poly.vertices[i][2] << ")";
        if (poly.patch)
            sout << ", normal: (" << poly.normals[i][0] << "," << poly.normals[i][1] << "," << poly.normals[i][2] << ")" << std::endl;
        sout << endl;
    }
    return sout;
}

////////////////////////////
/////// Sphere class: //////
////////////////////////////
HitRecord Sphere::intersect(Ray ray, double d0, double d1, bool debug) const {
    // based on p77 in textbook 
    HitRecord hit(SurfaceType::SPHERE, -1);

    // discriminant
    Vector3d diff = ray.eye - center;
    double disc = pow(ray.dir.dot(diff), 2) - (ray.dir.dot(ray.dir)) * (diff.dot(diff) - pow(radius,2));
    // potentially 2 solutions

    if (disc < 0) {
        return hit;  // no intersection
    }

    double numerator = (-1 * ray.dir).dot(diff); // part of numerator
    double denominator = ray.dir.dot(ray.dir);   // entire denominator
    if (disc == 0) {
        // ray grazes sphere and touches it in exactly one point
        hit.t = numerator / denominator;
    }
    else {
        // there are two solutions (where the ray enters and leaves)

        // TODO: if d0 cuts the sphere should we try using the larger solution?
        // we would want the smaller of the two solutions
        double t1 = (numerator - sqrt(disc)) / denominator;   // the smaller solution
        //double t2 = (numerator + sqrt(disc)) / denominator;   // the larger solution

        hit.t = t1;
    }

    // distance of intersection point away from ray.eye
    hit.point = ray.eye + hit.t * ray.dir;   // intersection point
    hit.dist = (hit.point - ray.eye).norm(); // actual distance (along ray) of intersection point
    if (! (d0 <= hit.dist && (hit.dist <= d1 || d1 == -1))) {
        // outside range [d0, d1]
        hit.t = -1;
        hit.dist = -1;
    }
    return hit;
}

/**
 * returns the normal of the surface at hit.point
 * where the given hit record is known to belong to this surface
 * and hit.t != -1
 */
Vector3d Sphere::getNormal(HitRecord hit, bool interpolate) const {
    return (hit.point - center);
}

/**
 * print out Sphere object for debugging
 */
std::ostream &operator<<(std::ostream &sout, const Sphere &sp) {
    sout << "Sphere: at (" << sp.center[0] << "," << sp.center[1] << "," << sp.center[2] << ")";
    sout << ", radius = " << sp.radius << endl;
    return sout;
}
