#ifndef _SPHERE_H_
#define  _SPHERE_H_
#include <Eigen/Dense>
#include <vector>
#include "Surface.h"
#include "Ray.h"
using Eigen::Vector3d;
/* File: Sphere.h
 *  Implements a class to represent a sphere in 3D space
 *  Dan Engbert - UMBC CMSC435 F18 Project 1
*/

class Sphere : public Surface {
    public:
        Sphere(Vector3d center, double radius, Vector3d color=Vector3d(0,0,0));
        double intersect(Ray ray, double hither=-1, bool debug=false) const;
        friend std::ostream &operator<<(std::ostream &sout, const Sphere &sp);

     private:
        Vector3d center;
        double radius;
        //Vector3d color;                  // color of this polygon (default: black)
};
#endif
