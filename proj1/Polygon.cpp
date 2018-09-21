#include "Polygon.h"
#include <iostream>
using Eigen::Vector3d;
using std::cout;
using std::endl;

/**
 * constuct a polygon object
 * for now this class only works for convex polygons
 *
 * vertices: vector of vertices (Vector3d objects) for this polygon
 * color:    the color of this polygon (1,1,1) for white
 *           (defaults to black (0,0,0))
 */
Polygon::Polygon(std::vector<Vector3d> vertices, Vector3d color) {
    setVertices(vertices);
    this->color = color;
}

void Polygon::setVertices(std::vector<Vector3d> vertices) {
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
 *
 * ray:     the ray to check for intersection with
 * hither:  ignore intersections that are closer than hither (-1 to disable)
 * debug:   whether or not to print out debug info for this intersection check
 */
double Polygon::intersect(Ray ray, double hither, bool debug) const {
    // distance along ray of closest triangle (-1 if none intersect)
    double dist = -1;
    for (unsigned int i=0; i<triangles.size(); i++) {
        // actual distance (along ray) of intersection point
        double res = triangles[i].intersect(ray, debug);

        if (res != -1 && (dist == -1 || res < dist)) {
            // we hit a triangle for the first time, or a closer triangle
            if (hither != -1 && res  < hither) {
                // don't count as intersection if it's closer than hither
                if (debug) {
                    cout << "\t(skipping result: " << res << " due to hither: " << hither << ")" << endl;
                }
                continue;
            }

            dist = res;
        }
    }
    return dist;
}

/**
 * return the triangle fan representing this polygon
 * (for debugging)
 */
std::vector<Triangle> Polygon::getTriangles() const {
    return triangles;
}

/**
 * print out the triangle fan for this polygon
 * (for debugging)
 */
void Polygon::printTriangles() const {
    std::cout << "Triangles in this polygon:" << std::endl;
    for (unsigned int i=0; i<triangles.size(); i++) {
        std::cout << "   " << triangles[i] << std::endl;
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
