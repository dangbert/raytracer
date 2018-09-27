#ifndef _POLYGON_H_
#define _POLYGON_H_
#include <Eigen/Dense>
#include <vector>
#include "Ray.h"
#include "Surface.h"
#include "Triangle.h"
using Eigen::Vector3d;
/* File: Polygon.h
 *  Implements a class to represent a polygon in 3D space
 *  Dan Engbert - UMBC CMSC435 F18 Project 1
*/

class Polygon : public Surface {
    public:
        Polygon(std::vector<Vector3d> vertices, Vector3d color=Vector3d(0,0,0));
        double intersect(Ray ray, double hither=-1, bool debug=false) const;
        friend std::ostream &operator<<(std::ostream &sout, const Polygon &poly);
        std::vector<Triangle> getTriangles() const;
        void printTriangles() const;

     private:
        void setVertices(std::vector<Vector3d> vertices);

        std::vector<Vector3d> vertices;  // vertices of triangle in 3D space
        std::vector<Triangle> triangles; // triangle fan for this polygon
};
#endif
