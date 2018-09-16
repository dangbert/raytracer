#include "Polygon.h"
#include <iostream>

Polygon::Polygon(std::vector<Eigen::Vector3d> vertices) {
    setVertices(vertices);
}

void Polygon::setVertices(std::vector<Eigen::Vector3d> vertices) {
    // TODO: if vertices.size < 3, throw error
    this->vertices = vertices;
    // create triangle fan
    triangles.clear();
    for (unsigned int i=2; i<vertices.size(); i++) {
        triangles.push_back(Triangle(vertices[0], vertices[i-1], vertices[i]));
    }
}
/**
 * returns distance along ray at which it intersects this polygon
 * (-1 if no intersection)
 * for now only works for convex polygons
 */
double Polygon::intersect(Ray ray) const {
    // TODO: use hither
    // distance along ray of closest triangle (-1 if none intersect)
    int dist = -1;
    for (unsigned int i=0; i<triangles.size(); i++) {
        int res = triangles[i].intersect(ray);
        if ((dist == -1) || (res != -1 && res < dist)) {
            dist = res;
        }
    }
    return dist;
}

/**
 * print out the triangle fan for this polygon
 * (for debugging)
 */
void Polygon::printTriangles() const {
    for (unsigned int i=0; i<triangles.size(); i++) {
        std::cout << "triangle" << i << ":" << std::endl;
        std::cout << triangles[i] << std::endl;
    }
}

// print out this object for debugging
std::ostream& operator<<(std::ostream &sout, const Polygon &poly) {
    sout << "Polygon (" << poly.vertices.size() << " vertices):" << std::endl;
    for (unsigned int i = 0; i != poly.vertices.size(); i++) {
        sout << "   ";
        sout << poly.vertices[i][0] << "\t" << poly.vertices[i][1] << "\t" << poly.vertices[i][2] << "\t" << std::endl;
    }
    return sout;
}
