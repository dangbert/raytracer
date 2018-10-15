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

// stores info about the Rasterization setup
struct RasterizerSettings {
    Vector3d eye; // camera origin
    // vectors defining camera space:
    Vector3d w;
    Vector3d u;
    Vector3d v;
    double h;
    // planes of view frustum:
    double right; // right plane
    double left;
    double top;
    double bottom;
    double near;  // near plane
    double far;
};

struct Fragment {
    double zValue;     // interpolated zValue (used ONLY for depth ordering)
    Vector3d color;    // interpolated color
    Vector3d normal;   // interpolated, in world space
    Vector3d worldPos; // position in world space
    struct Fragment *next;
};

class Rasterizer {
    friend class Test;
    public:
        Rasterizer(std::string filename);
        void render(std::string filename, bool debug=false);

    private:
        Eigen::Matrix4d createMatrix(bool debug=false);
        void vertexProcessing(Eigen::Matrix4d M);
        Vector3d shadePoint(Triangle &tri, int vertex);
        HitRecord getHitRecord(Ray ray, double d0, double d1);
        void rasterization(struct Fragment ***frags);
        void fragmentProcessing(struct Fragment ***frags);
        void blending(unsigned char* pixels, struct Fragment ***frags);

        std::vector<Triangle> triangles;        // vector of all triangles in scene
        SettingsNFF nff;
        struct RasterizerSettings scene;
};
#endif
