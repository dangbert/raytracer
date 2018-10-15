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
class Rasterizer;
class HitRecord;

/**
 *  class for geometric surfaces to derive from
 *  a surface should not be modified after creating it
 */
class Surface {
    friend class Test;
    friend class RayTracer;
    friend class Rasterizer;
    public:
        Surface(Material *matr) : matr(matr) {};
        virtual ~Surface() {} // (needed so we can call delete on a *Surface)
        // TODO: consider adding params t0 and t1 (hither) to specify bounds
        virtual HitRecord intersect(Ray ray, double d0=0, double d1=-1, bool debug=false) const = 0;
        virtual Vector3d getNormal(HitRecord hit, bool interpolate=false) const = 0;

     protected:
        Material *matr;         // material for this surface
};

/**
 *  class to represent a triangle in 3D space
 *  a triangle patch is a triangle where the normals of the surface at each vertex are known.
 */
class Triangle : public Surface {
    friend class Test;
    friend class Rasterizer;
    public:
        Triangle(Vector3d points[3], Material *matr=NULL);
        Triangle(Vector3d points[3], Vector3d norms[3], Material *matr=NULL);
        HitRecord intersect(Ray ray, double d0=0, double d1=-1, bool debug=false) const;
        Vector3d getNormal(HitRecord hit, bool interpolate=false) const;
        inline bool isPatch() {return patch;};
        friend std::ostream &operator<<(std::ostream &sout, const Triangle &tri);
   private:
        Vector3d points[3]; // vertices of triangle in 3D space
        bool patch;         // whether or not the normals at the vertices are known
        Vector3d norms[3];  // normal at each vertex (not used if patch==true)
        Vector3d colors[3]; // color of each vertex (for rasterization)
        Vector3d imgPoints[3]; // vertices of triangle in image space (for rasterization)
};

/**
 *  class to represent a polygon in 3D space
 *  only works for convex polygons at the moment
 *
 *  a polygon patch is a triangle where the normals of the surface at each vertex are known.
 */
class Polygon : public Surface {
    friend class Test;
    friend class Rasterizer;
    public:
        Polygon(Material *matr, std::vector<Vector3d> vertices, bool patch=false, std::vector<Vector3d> normals=std::vector<Vector3d>());
        HitRecord intersect(Ray ray, double d0=0, double d1=-1, bool debug=false) const;
        Vector3d getNormal(HitRecord hit, bool interpolate=false) const;
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
        Vector3d getNormal(HitRecord hit, bool interpolate=false) const;
        friend std::ostream &operator<<(std::ostream &sout, const Sphere &sp);

     private:
        Vector3d center;
        double radius;
};
#endif
