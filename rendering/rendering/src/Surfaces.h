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
 *  Abstract class for geometric surfaces to derive from.
 *  Surfaces may have Material properties, and implement intersect() and getNormal() functions.
 * 
 *  A surface should not be modified after creating it.
 *  TOOD: consider making what is needed public,
 *    so client apps like Raytracer and Rasterizer can access what they need
 *    (without being friends)
 */
class Surface {
    friend class Test;
    friend class RayTracer;
    friend class Rasterizer;
    public:
        Surface(Material *_matr) : matr(_matr) {}
        virtual ~Surface() {} // (needed so we can call delete on a *Surface)
        Surface(const Surface &other) = default;
        // TODO: consider adding params t0 and t1 (hither) to specify bounds
        virtual HitRecord intersect(const Ray &ray, double d0=0, double d1=-1, bool debug=false) const = 0;
        virtual Vector3d getNormal(HitRecord hit, bool interpolate=false) const = 0;

    protected:
        // TOOD: consider writing all member variables in my classes like m_matr
        //   (also removes need to use underscore in constructors for shadow warning...)
        // TODO: use shared or unique pointers for situtations like this (to make it clear who owns it)
        //   (and also look up weak pointers)
        // TODO: maybe make this const...
        Material *matr;         //!< material property of this surface
};

/**
 * Represents a triangle in 3D space.
 * Note: a triangle patch is a triangle where the normals of the surface at each vertex are known.
 */
class Triangle final : public Surface {
    friend class Test;
    friend class Rasterizer;
    public:
        // TODO: start using nullptr (because NULL is less typesafe as an int...)
        Triangle(Vector3d _points[3], Material *_matr=NULL);
        Triangle(Vector3d _points[3], Vector3d _norms[3], Material *_matr=NULL);
        HitRecord intersect(const Ray &ray, double d0=0, double d1=-1, bool debug=false) const override;
        Vector3d getNormal(HitRecord hit, bool interpolate=false) const override;
        inline bool isPatch() {return patch;}
        friend std::ostream &operator<<(std::ostream &sout, const Triangle &tri);
   private:
        // TODO: consider using std::array (which is smarter than carrays, tracks its size)
        //   ^and safer for usage as a param in Triangle constructor (ensuring param array is actually right size)
        //   and use points.at(2) for indexing instead of [2] (more memory safe)
        //std::array<Vector3d, 3> points;
        Vector3d points[3]; // vertices of triangle in 3D space
        bool patch;         // whether or not the normals at the vertices are known
        Vector3d norms[3];  // normal at each vertex (not used if patch==true)
        Vector3d colors[3]; // color of each vertex (for rasterization)
        Vector3d imgPoints[3]; // vertices of triangle in image space (for rasterization)
};

/**
 *  Represents a polygon in 3D space.
 *  (Only works for convex polygons at the moment)
 *
 *  Note: A polygon patch is a triangle where the normals of the surface at each vertex are known.
 */
class Polygon final : public Surface {
    friend class Test;
    friend class Rasterizer;
    public:
        Polygon(Material *matr, std::vector<Vector3d> vertices, bool patch=false, std::vector<Vector3d> normals=std::vector<Vector3d>());
        HitRecord intersect(const Ray &ray, double d0=0, double d1=-1, bool debug=false) const override;
        Vector3d getNormal(HitRecord hit, bool interpolate=false) const override;
        friend std::ostream &operator<<(std::ostream &sout, const Polygon &poly);
        void printTriangles() const;
        inline bool isPatch() {return patch;}

    protected:
        std::vector<Vector3d> vertices;  // vertices of triangle in 3D space
        std::vector<Triangle> triangles; // triangle fan for this polygon
        bool patch;
        std::vector<Vector3d> normals;   // normals of vertices (if we're a polygon patch)
};

/**
 *  Represents a sphere in 3D space.
 */
class Sphere final : public Surface {
    friend class Test;
    public:
        Sphere(Material *_matr, Vector3d _center, double _radius)
            : Surface(_matr), center(_center), radius(_radius) {}
        HitRecord intersect(const Ray &ray, double d0=0, double d1=-1, bool debug=false) const override;
        Vector3d getNormal(HitRecord hit, bool interpolate=false) const override;
        friend std::ostream &operator<<(std::ostream &sout, const Sphere &sp);

     private:
        Vector3d center;
        double radius;
};
#endif
