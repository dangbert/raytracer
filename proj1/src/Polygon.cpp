#include "Polygon.h"

Polygon::Polygon(std::vector<Eigen::Vector3d> vertices) {
    this->vertices = vertices;
}

/**
 * returns distance along ray at which it intersects this polygon
 * (-1 if no intersection)
 * for now only works for convex polygons
 */
double Polygon::intersect(Ray ray) {
    // TODO: implement
    // split into triangles (make sure we only have to do this once)
    // make use of Triangle.intersect

}
