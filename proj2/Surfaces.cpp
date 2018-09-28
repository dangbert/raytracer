#include <Eigen/Dense>
#include <iostream>
#include "Surfaces.h"

using Eigen::Vector3d;
using std::cout;
using std::endl;

////////////////////////////
///// Triangle class: //////
////////////////////////////

Triangle::Triangle(Eigen::Vector3d p1, Eigen::Vector3d p2, Eigen::Vector3d p3) {
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
}

/*
 * returns distance along ray at which it intersects this triangle
 * (-1 if no intersection)
 */
double Triangle::intersect(Ray ray, bool debug) const {
    double vals[4];
    intersectValues(ray, vals, debug); // [B, g, t, dist]
    return vals[3];
}

/*
 * helper function for intersect()
 * vals: array of length 3 to store result [B, g, t, dist]
 *
 * calculates the values B, g, t that are the solution to
 *   the intersection of the ray and this triangle
 *   and the distance (dist) along the ray at which the interesection occurs
 *   (-1 if no intersection)
 */
void Triangle::intersectValues(Ray ray, double vals[], bool debug) const {
    // ray: e + td
    // triangle a + B(b-a) + g(c-a)
    // solve system: Ax=b
    // using Cramer's rule (see textbook p. 78)
    if (debug)
        cout << "\nintersecting " << *this << " with " << ray << endl;

    Eigen::Matrix3d A, M1, M2, M3;
    Vector3d b;
    A << p1-p2, p1-p3, ray.dir;

    // TODO: check matrices have determinants?
    M1 << p1-ray.eye, p1-p3, ray.dir;
    M2 << p1-p2, p1-ray.eye, ray.dir;
    M3 << p1-p2, p1-p3, p1-ray.eye;
    if (debug)
        cout << "A =\n " << A << endl;
    double det = A.determinant();
    double B = M1.determinant() / det;
    double g = M2.determinant() / det;
    double t = M3.determinant() / det;
    vals[0] = B;
    vals[1] = g;
    vals[2] = t;

    // check conditions to see if solution is valid
    if (t>0 && 0<=B && 0<=g && B+g<=1) {
        Vector3d point = ray.eye + t * ray.dir; // intersection point
        vals[3] = (point - ray.eye).norm();     // distance along ray (to intersection point)
    }
    else {
        vals[3] = -1;
    }
    if (debug)
        cout << "\tt=" << t << ", B=" << B << ", g=" << g << "\tdist=" << vals[3] << endl;
}

// print out this object for debugging
std::ostream &operator<<(std::ostream &sout, const Triangle &tri) {
    sout << "Triangle: ";
    sout << "(" << tri.p1[0] << "," << tri.p1[1] << "," << tri.p1[2] << "), ";
    sout << "(" << tri.p2[0] << "," << tri.p2[1] << "," << tri.p2[2] << "), ";
    sout << "(" << tri.p3[0] << "," << tri.p3[1] << "," << tri.p3[2] << ")";
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
Polygon::Polygon(std::vector<Vector3d> vertices, Vector3d color)
{
    this->color = color;
    setVertices(vertices);
}

void Polygon::setVertices(std::vector<Vector3d> vertices) {
    // TODO: if vertices.size < 3, throw error
    this->vertices = vertices;
    // create triangle fan
    triangles.clear();
    for (unsigned int i=2; i<vertices.size(); i++) {
        triangles.push_back(Triangle(vertices[0], vertices[i-1], vertices[i]));
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
double Polygon::intersect(Ray ray, double hither, bool debug) const {
    // distance along ray of closest triangle (-1 if none intersect)
    double dist = -1;
    for (unsigned int i=0; i<triangles.size(); i++) {
        // actual distance (along ray) of intersection point
        double res = triangles[i].intersect(ray, debug);

        if (res != -1 && (dist == -1 || res < dist)) {
            // we hit a triangle for the first time, or a closer triangle
            if (hither != -1 && res  < hither) {
                // don't count as intersection if it's closer than hither
                if (debug) {
                    cout << "\t(skipping result: " << res << " due to hither: " << hither << ")" << endl;
                }
                continue;
            }

            dist = res;
        }
    }
    return dist;
}

/**
 * return the triangle fan representing this polygon
 * (for debugging)
 */
std::vector<Triangle> Polygon::getTriangles() const {
    return triangles;
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
 * print out this object for debugging
 */
std::ostream& operator<<(std::ostream &sout, const Polygon &poly) {
    sout << "Polygon (" << poly.vertices.size() << " vertices):" << std::endl;
    for (unsigned int i = 0; i != poly.vertices.size(); i++) {
        sout << "   ";
        sout << poly.vertices[i][0] << "\t" << poly.vertices[i][1] << "\t" << poly.vertices[i][2] << "\t" << std::endl;
    }
    return sout;
}



////////////////////////////
/////// Sphere class: //////
////////////////////////////
Sphere::Sphere(Vector3d center, double radius, Vector3d color)
{
    this->color = color;
    this->center = center;
    this->radius = radius;
}

double Sphere::intersect(Ray ray, double hither, bool debug) const {
    // based on p77 in textbook 

    // discriminant
    Vector3d diff = ray.eye - center;
    double disc = pow(ray.dir.dot(diff), 2) - (ray.dir.dot(ray.dir)) * (diff.dot(diff) - pow(radius,2));
    // potentially 2 solutions

    if (disc < 0) {
        return -1;  // no intersection
    }

    double numerator = (-1 * ray.dir).dot(diff); // part of numerator
    double denominator = ray.dir.dot(ray.dir);   // entire denominator
    if (disc == 0) {
        // ray grazes sphere and touches it in exactly one point
        return numerator / denominator;
    }
    else {
        // there are two solutions (where the ray enters and leaves)

        // TODO: if hither cuts the sphere should we try using the larger solution?
        // we would want the smaller of the two solutions
        double t1 = (numerator - sqrt(disc)) / denominator;   // the smaller solution
        //double t2 = (numerator + sqrt(disc)) / denominator;   // the larger solution

        // distance of intersection point away from ray.eye
        double realDist = (t1 * ray.dir).norm();
        if (hither != -1 && realDist < hither) {
            return -1;
        }
        return t1;
    }
}

/**
 * print out this object for debugging
 */
std::ostream &operator<<(std::ostream &sout, const Sphere &sp) {
    sout << "Sphere: at (" << sp.center[0] << "," << sp.center[1] << "," << sp.center[2] << ")";
    sout << ", radius = " << sp.radius << endl;
    return sout;
}
