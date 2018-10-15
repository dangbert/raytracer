#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_
/* File: Rasterizer.h
 * This file defines classes necessary for Rasterizing
 * Dan Engbert - UMBC CMSC435 F18 Project 1
 */
#define SHADOW_BIAS 1.0e-6

#include <Eigen/Dense>
#include "Settings.h"
#include "Surfaces.h"
using Eigen::Vector3d;

// forward declaration
class Test;

struct RasterizerSettings {
    Vector3d eye;
    Vector3d w;
    Vector3d u;
    Vector3d v;
    double h;
    double right;
    double left;
    double top;
    double bottom;
    double near;
    double far;
};

class Rasterizer {
    friend class Test;
    public:
        Rasterizer(std::string filename);
        void render(std::string filename, bool debug=false);

    private:
        Eigen::Matrix4d createMatrix(bool debug=false);
        void vertexProcessing(Eigen::Matrix4d M);
        Vector3d shadePoint(Triangle &tri, int vertex, Material *matr);
        HitRecord getHitRecord(Ray ray, double d0, double d1);
        std::vector<Triangle> triangles;        // vector of all triangles in scene
        SettingsNFF nff;
        struct RasterizerSettings scene;
};
#endif
