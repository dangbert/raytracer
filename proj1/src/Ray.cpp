#include "Ray.h"
#include <iostream>

Ray::Ray(Eigen::Vector3d eye, Eigen::Vector3d dir) {
    this->eye = eye;
    this->dir = dir;
}
