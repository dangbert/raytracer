#include "Sphere.h"
#include <iostream>
#include <math.h>
using Eigen::Vector3d;
using std::cout;
using std::endl;

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
