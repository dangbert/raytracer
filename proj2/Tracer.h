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
        Ray(Eigen::Vector3d eye, Eigen::Vector3d dir) : eye(eye), dir(dir) {};
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
        HitRecord(SurfaceType sType, double t, double dist=-1, Vector3d point=Vector3d(0,0,0), double B=0, double g=0, double triIndex=-1, double surfIndex=-1)
            : sType(sType), t(t), dist(dist), point(point), B(B), g(g),
            triIndex(triIndex), surfIndex(surfIndex) {};
        friend std::ostream &operator<<(std::ostream &sout, const HitRecord &hit);

        SurfaceType sType; // enum surface type
        double t;          // t value for intersection along ray (-1 if no intersection)
        double dist;       // actual distance along ray of intersection (calculated with t)
        Vector3d point;    // intersection point
        double B;          // triangle intersection Beta value
        double g;          // triangle intersectiong gamma value
        // TODO: store pointer to material intersected with?
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
        Eigen::Vector3d trace(Ray ray, int bounces=0, bool debug=false);
        HitRecord getHitRecord(Ray ray, double d0, double d1, bool debug);
};
#endif
