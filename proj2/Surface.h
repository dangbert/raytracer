#ifndef _SURFACE_H_
#define _SURFACE_H_
#include <Eigen/Dense>
#include <vector>
#include "Ray.h"
using Eigen::Vector3d;
/* File: Surface.h
 *  Implements an abstract class for geometric objects to derive from
 *  Dan Engbert - UMBC CMSC435 F18 Project 1
*/

class Surface {
    public:
        Surface(Vector3d color=Vector3d(0,0,0));
        virtual ~Surface() {} // (needed so we can call delete on a *Surface)
        virtual double intersect(Ray ray, double hither=-1, bool debug=false) const = 0;

     private:
        friend class RayTracer;

     protected:
        Vector3d color;                  // color of this polygon (default: black)
};
#endif
