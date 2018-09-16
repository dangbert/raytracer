#include "Triangle.h"
#include <iostream>

Triangle::Triangle(Eigen::Vector3d p1, Eigen::Vector3d p2, Eigen::Vector3d p3) {
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
}

/*
 * returns distance along ray at which it intersects this triangle
 * (-1 if no intersection)
 */
double Triangle::intersect(Ray ray) const {
    // TODO: use hither
    // ray: e + td
    // triangle a + B(b-a) + g(c-a)
    // solve system: Ax=b
    Eigen::Matrix3d A;
    Eigen::Vector3d b;
    A << -ray.dir, (p2 - p1), (p3 - p1);
    b << ray.eye - p1;
    //std::cout << "A:\n" << A << std::endl;
    // solve system of equations
    // intersection point: x = (t, B, g)
    // TODO: what if there is no solution?
    // (TODO: use Kramer's rule instead)
    Eigen::Vector3d x = A.colPivHouseholderQr().solve(b);

    // check conditions to see if solution is valid
    if (0<=x[1] && 0<=x[2] && x[2]+x[1]<=1 && x[0]>0) {
        return (x - ray.eye).norm();
    }
    return -1;
}

// print out this object for debugging
std::ostream &operator<<(std::ostream &sout, const Triangle &tri) {
    sout << "Triangle: ";
    sout << "(" << tri.p1[0] << "," << tri.p1[1] << "," << tri.p1[2] << "), ";
    sout << "(" << tri.p2[0] << "," << tri.p2[1] << "," << tri.p2[2] << "), ";
    sout << "(" << tri.p3[0] << "," << tri.p3[1] << "," << tri.p3[2] << std::endl;
    return sout;
}
