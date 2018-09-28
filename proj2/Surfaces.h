#ifndef _SURFACES_H_
#define _SURFACES_H_
/* File: Surfaces.h
 * This file defines classes for geometric objects
 *  Dan Engbert - UMBC CMSC435 F18 Project 1
 */
#include <Eigen/Dense>
#include <vector>
#include "Tracer.h"
#include "Settings.h"
using Eigen::Vector3d;

// forward delcaration
class Ray;
class Test;

/**
 *  class to represent a triangle in 3D space
 */
class Triangle {
    friend class Test;
    public:
        Triangle(Vector3d p1, Vector3d p2, Vector3d p3)
            : p1(p1), p2(p2), p3(p3) {};
        double intersect(Ray ray, bool debug=false) const;
        void intersectValues(Ray ray, double vals[], bool debug=false) const;
        friend std::ostream &operator<<(std::ostream &sout, const Triangle &tri);

        // vertices of triangle in 3D space
        Vector3d p1;
        Vector3d p2;
        Vector3d p3;
};

/**
 *  class for geometric surfaces to derive from
 */
class Surface {
    friend class RayTracer;
    friend class Test;
    public:
        Surface(Material &matr) : matr(matr) {};
        virtual ~Surface() {} // (needed so we can call delete on a *Surface)
        virtual double intersect(Ray ray, double hither=-1, bool debug=false) const = 0;

     protected:
        Material &matr;         // material for this surface
};

/**
 *  class to represent a polygon in 3D space
 *  only works for convex polygons at the moment
 */
class Polygon : public Surface {
    friend class Test;
    public:
        Polygon(Material &matr, std::vector<Vector3d> vertices);
        double intersect(Ray ray, double hither=-1, bool debug=false) const;
        friend std::ostream &operator<<(std::ostream &sout, const Polygon &poly);
        std::vector<Triangle> getTriangles() const;
        void printTriangles() const;

     private:
        std::vector<Vector3d> vertices;  // vertices of triangle in 3D space
        std::vector<Triangle> triangles; // triangle fan for this polygon
};

/**
 *  Implements a class to represent a sphere in 3D space
 */
class Sphere : public Surface {
    friend class Test;
    public:
        Sphere(Material &matr, Vector3d center, double radius)
            : Surface(matr), center(center), radius(radius) {};
        double intersect(Ray ray, double hither=-1, bool debug=false) const;
        friend std::ostream &operator<<(std::ostream &sout, const Sphere &sp);

     private:
        Vector3d center;
        double radius;
};
#endif
