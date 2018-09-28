#ifndef _TRACER_H_
#define _TRACER_H_
/* File: RayTracer.h
 * This file defines classes necessary for ray tracing
 * Dan Engbert - UMBC CMSC435 F18 Project 1
 */

#include <Eigen/Dense>
#include "Settings.h"
#include "Surfaces.h"

// forward declaration
class Test;

class Ray {
    friend class Test;
    public:
        Ray(Eigen::Vector3d eye, Eigen::Vector3d dir) : eye(eye), dir(dir) {};
        friend std::ostream &operator<<(std::ostream &sout, const Ray &ray);

        Eigen::Vector3d eye;   // eye
        Eigen::Vector3d dir;   // direction
};

class RayTracer {
    friend class Test;
    public:
        RayTracer(std::string filename);
        void render(std::string filename, bool debug=false);
        void animate(std::string filename, bool debug=false);

    private:
        SettingsNFF nff;
        Eigen::Vector3d trace(Ray ray, bool debug=false);
};
#endif
