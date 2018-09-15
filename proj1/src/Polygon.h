#ifndef _POLYGON_H_
#define _POLYGON_H_
#include <Eigen/Dense>
#include <vector>
#include "Ray.h"
/* File: Polygon.h
 *  Implements a class to represent a polygon in 3D space
 *  Dan Engbert - UMBC CMSC435 F18 Project 1
*/

class Polygon {
    public:
        Polygon(std::vector<Eigen::Vector3d> vertices);
        double intersect(Ray ray);

        // vertices of triangle in 3D space
        std::vector<Eigen::Vector3d> vertices;
        // TODO: store fill color for this polygon as well???
};
#endif
