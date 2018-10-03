#ifndef _SURFACES_H_
#define _SURFACES_H_
/* File: Surfaces.h
 * This file defines classes for geometric objects
 *  Dan Engbert - UMBC CMSC435 F18 Project 1
 */
#include <Eigen/Dense>
#include <vector>

enum class SurfaceType {TRIANGLE, POLYGON, SPHERE, POLYGONPATCH};
#include "Tracer.h"
#include "Settings.h"
using Eigen::Vector3d;

// forward delcaration
class Ray;
class Test;
class HitRecord;

/**
 *  class to represent a triangle in 3D space
 *  a triangle patch is a triangle where the normals of the surface at each vertex are known.
 * TODO TODO: consider making this a child of Surface (storing an extra Material* is not inefficient)
 */
class Triangle {
    friend class Test;
    public:
        Triangle(Vector3d p1, Vector3d p2, Vector3d p3, bool patch=false,
                Vector3d n1=Vector3d(0,0,0), Vector3d n2=Vector3d(0,0,0), Vector3d n3=Vector3d(0,0,0))
            : p1(p1), p2(p2), p3(p3), patch(patch), n1(n1), n2(n2), n3(n3) {};
        HitRecord intersect(Ray ray, double d0=0, double d1=-1, bool debug=false) const;
        Vector3d getNormal(HitRecord hit) const;
        inline bool isPatch() {return patch;};
        friend std::ostream &operator<<(std::ostream &sout, const Triangle &tri);
   private:
        // vertices of triangle in 3D space
        Vector3d p1;
        Vector3d p2;
        Vector3d p3;
        bool patch;
        Vector3d n1;
        Vector3d n2;
        Vector3d n3;
};

/**
 *  class for geometric surfaces to derive from
 *  a surface should not be modified after creating it
 */
class Surface {
    friend class RayTracer;
    friend class Test;
    public:
        Surface(Material *matr) : matr(matr) {};
        virtual ~Surface() {} // (needed so we can call delete on a *Surface)
        // TODO: consider adding params t0 and t1 (hither) to specify bounds
        virtual HitRecord intersect(Ray ray, double d0=0, double d1=-1, bool debug=false) const = 0;
        virtual Vector3d getNormal(HitRecord hit) const = 0;

     protected:
        Material *matr;         // material for this surface
};

/**
 *  class to represent a polygon in 3D space
 *  only works for convex polygons at the moment
 *
 *  a polygon patch is a triangle where the normals of the surface at each vertex are known.
 */
class Polygon : public Surface {
    friend class Test;
    public:
        Polygon(Material *matr, std::vector<Vector3d> vertices, bool patch=false, std::vector<Vector3d> normals=std::vector<Vector3d>());
        HitRecord intersect(Ray ray, double d0=0, double d1=-1, bool debug=false) const;
        Vector3d getNormal(HitRecord hit) const;
        friend std::ostream &operator<<(std::ostream &sout, const Polygon &poly);
        void printTriangles() const;
        inline bool isPatch() {return patch;};

    protected:
        std::vector<Vector3d> vertices;  // vertices of triangle in 3D space
        std::vector<Triangle> triangles; // triangle fan for this polygon
        bool patch;
        std::vector<Vector3d> normals;   // normals of vertices (if we're a polygon patch)
};

/**
 *  Implements a class to represent a sphere in 3D space
 */
class Sphere : public Surface {
    friend class Test;
    public:
        Sphere(Material *matr, Vector3d center, double radius)
            : Surface(matr), center(center), radius(radius) {};
        HitRecord intersect(Ray ray, double d0=0, double d1=-1, bool debug=false) const;
        Vector3d getNormal(HitRecord hit) const;
        friend std::ostream &operator<<(std::ostream &sout, const Sphere &sp);

     private:
        Vector3d center;
        double radius;
};
#endif
