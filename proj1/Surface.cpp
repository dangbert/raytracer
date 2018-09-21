#include <Eigen/Dense>
#include <vector>
#include "Surface.h"
using Eigen::Vector3d;

Surface::Surface() {
    color = Vector3d(0,0,0);
}
Surface::Surface(Vector3d color) {
    this->color = color;
}
