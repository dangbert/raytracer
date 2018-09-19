#include "Triangle.h"
#include <iostream>
using std::cout;
using std::endl;

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
