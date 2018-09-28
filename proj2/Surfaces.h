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

/**
 *  class to represent a triangle in 3D space
 */
class Triangle {
    public:
        Triangle(Vector3d p1, Vector3d p2, Vector3d p3);
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
    public:
        Surface(Vector3d color=Vector3d(0,0,0)) : color(color) {};
        virtual ~Surface() {} // (needed so we can call delete on a *Surface)
        virtual double intersect(Ray ray, double hither=-1, bool debug=false) const = 0;

     private:
        friend class RayTracer;

     protected:
        Vector3d color;                  // color of this polygon (default: black)
};

/**
 *  class to represent a polygon in 3D space
 */
class Polygon : public Surface {
    public:
        Polygon(std::vector<Vector3d> vertices, Vector3d color=Vector3d(0,0,0));
        double intersect(Ray ray, double hither=-1, bool debug=false) const;
        friend std::ostream &operator<<(std::ostream &sout, const Polygon &poly);
        std::vector<Triangle> getTriangles() const;
        void printTriangles() const;

     private:
        void setVertices(std::vector<Vector3d> vertices);

        std::vector<Vector3d> vertices;  // vertices of triangle in 3D space
        std::vector<Triangle> triangles; // triangle fan for this polygon
};

/**
 *  Implements a class to represent a sphere in 3D space
 */
class Sphere : public Surface {
    public:
        Sphere(Vector3d center, double radius, Vector3d color=Vector3d(0,0,0));
        double intersect(Ray ray, double hither=-1, bool debug=false) const;
        friend std::ostream &operator<<(std::ostream &sout, const Sphere &sp);

     private:
        Vector3d center;
        double radius;
};
#endif
