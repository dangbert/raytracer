#include "Ray.h"
#include <iostream>

/*
 * constructs a Ray object
 *
 * a ray starts at a give 3D point (eye)
 * and moves out from that point in a straight line parallel to
 * the vector dir
 */
Ray::Ray(Eigen::Vector3d eye, Eigen::Vector3d dir) {
    this->eye = eye;
    this->dir = dir;
}

/*
 * print out this Ray object for debugging
 */
std::ostream &operator<<(std::ostream &sout, const Ray &ray) {
    sout << "Ray: ";
    sout << "(" << ray.eye[0] << "," << ray.eye[1] << "," << ray.eye[2] << ") -> ";
    sout << "(" << ray.dir[0] << "," << ray.dir[1] << "," << ray.dir[2] << ")";
    return sout;
}
