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

class Rasterizer {
    friend class Test;
    public:
        Rasterizer(std::string filename);
        void render(std::string filename, bool debug=false);

    private:
        SettingsNFF nff;
        Eigen::Matrix4d createMatrix(Vector3d u, Vector3d v, Vector3d w, bool debug=false);
};
#endif
