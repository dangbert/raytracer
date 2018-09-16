#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_
#include <Eigen/Dense>
#include "SettingsNFF.h"
#include "Ray.h"
#include "Polygon.h"
#include "Triangle.h"
/* File: RayTracer.h
    Dan Engbert - UMBC CMSC435 F18 Project 1
*/

class RayTracer {
    public:
        RayTracer(SettingsNFF nff);
        void render(std::string filename);

    private:
        SettingsNFF nff;
        Eigen::Vector3d trace(Ray ray);
};
#endif
