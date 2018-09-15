#ifndef _RAY_H_
#define _RAY_H_
#include <Eigen/Dense>
/* File: Ray.h
 *  Implements a class to represent a 3D ray
 *  Dan Engbert - UMBC CMSC435 F18 Project 1
*/

class Ray {
    public:
        Ray(Eigen::Vector3d eye, Eigen::Vector3d dir);

        Eigen::Vector3d eye;   // eye
        Eigen::Vector3d dir;   // direction
};
#endif
