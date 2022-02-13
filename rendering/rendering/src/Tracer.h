#ifndef _TRACER_H_
#define _TRACER_H_
/* File: RayTracer.h
 * This file defines classes necessary for ray tracing
 * Dan Engbert - UMBC CMSC435 F18 Project 1
 */
#define SHADOW_BIAS 1.0e-6

#include <Eigen/Dense>
#include "Settings.h"
#include "Surfaces.h"
using Eigen::Vector3d;

// forward declaration
class Test;

/**
 * class to represent a ray in 3d space
 * points on ray fit: seye + t * dir (for any t)
 */
class Ray {
    friend class Test;
    public:
        Ray(Eigen::Vector3d _eye, Eigen::Vector3d _dir)
            : eye(_eye), dir(_dir) {}
        friend std::ostream &operator<<(std::ostream &sout, const Ray &ray);

        Eigen::Vector3d eye;   // eye
        Eigen::Vector3d dir;   // direction
};

/**
 * class for storing info about the intersection of a ray with a surface
 * t is -1 if no intersection occurred
 */
class HitRecord {
    friend class Test;
    public:
        HitRecord(SurfaceType _sType, double _t, double _dist=-1, Vector3d _point=Vector3d(0,0,0), double _a=0, double _B=0, double _g=0, int _triIndex=-1, int _surfIndex=-1)
            : sType(_sType), t(_t), dist(_dist), point(_point), a(_a), B(_B), g(_g),
            triIndex(_triIndex), surfIndex(_surfIndex) {}
        friend std::ostream &operator<<(std::ostream &sout, const HitRecord &hit);

        SurfaceType sType; // enum surface type
        double t;          // t value for intersection along ray (-1 if no intersection)
        double dist;       // actual distance along ray of intersection (calculated with t)
        Vector3d point;    // intersection point
        double a;          // triangle intersection alpha value
        double B;          // triangle intersection Beta value
        double g;          // triangle intersectiong gamma value
        // TODO: store pointer to material intersected with?
        // TODO: instead of using -1 do something like:
        //   std::optional<int> triIndex
        //   then later if (triIndex.has_value()) and triIndex.value()
        int triIndex;      // index of triangle intersected (if sTYPE == POLYGON)
        int surfIndex;     // index of the surface intersected
};

class RayTracer {
    friend class Test;
    public:
        RayTracer(std::string filename);
        void render(std::string filename, int bounces, bool debug=false);
        void animate(std::string filename, bool debug=false);

    private:
        SettingsNFF nff;
        Eigen::Vector3d trace(Ray ray, double d0, double d1, int bounces=0, bool debug=false);
        HitRecord getHitRecord(Ray ray, double d0, double d1, bool debug);
};
#endif
