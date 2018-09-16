#ifndef _Triangle_H_
#define _Triangle_H_
#include <Eigen/Dense>
#include "Ray.h"
/* File: Triangle.h
 *  Implements a class to represent a triangle in 3D space
 *  Dan Engbert - UMBC CMSC435 F18 Project 1
*/

class Triangle {
    public:
        Triangle(Eigen::Vector3d p1, Eigen::Vector3d p2, Eigen::Vector3d p3);
        double intersect(Ray ray) const;
        friend std::ostream &operator<<(std::ostream &sout, const Triangle &tri);

        // vertices of triangle in 3D space
        Eigen::Vector3d p1;
        Eigen::Vector3d p2;
        Eigen::Vector3d p3;
};
#endif
