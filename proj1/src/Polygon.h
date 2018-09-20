#ifndef _POLYGON_H_
#define _POLYGON_H_
#include <Eigen/Dense>
#include <vector>
#include "Ray.h"
#include "Triangle.h"
using Eigen::Vector3d;
/* File: Polygon.h
 *  Implements a class to represent a polygon in 3D space
 *  Dan Engbert - UMBC CMSC435 F18 Project 1
*/

class Polygon {
    public:
        Polygon(std::vector<Eigen::Vector3d> vertices);
        double intersect(Ray ray, double hither=-1, bool debug=false) const;
        friend std::ostream &operator<<(std::ostream &sout, const Polygon &poly);
        std::vector<Triangle> getTriangles() const;
        void printTriangles() const;

     private:
        void setVertices(std::vector<Eigen::Vector3d> vertices);

        // TODO: store fill color for this polygon as well???
        // vertices of triangle in 3D space
        std::vector<Eigen::Vector3d> vertices;
        // triangle fan for this polygon
        std::vector<Triangle> triangles;
};
#endif
