#ifndef _Triangle_H_
#define _Triangle_H_
#include <Eigen/Dense>
#include "Ray.h"
using Eigen::Vector3d;
/* File: Triangle.h
 *  Implements a class to represent a triangle in 3D space
 *  Dan Engbert - UMBC CMSC435 F18 Project 1
*/

class Triangle {
    public:
        Triangle(Vector3d p1, Vector3d p2, Vector3d p3);
        double intersect(Ray ray, bool debug=false) const;
        void intersectValues(Ray ray, double vals[], bool debug=false) const;
        friend std::ostream &operator<<(std::ostream &sout, const Triangle &tri);

        // vertices of triangle in 3D space
        Vector3d p1;
        Vector3d p2;
        Vector3d p3;
};
#endif
