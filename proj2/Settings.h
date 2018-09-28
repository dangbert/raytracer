#ifndef _SETTINGS_H_
#define _SETTINGS_H_
/* File: Settings.h
 * This file defines classes for storing data from NFF files
 * Dan Engbert - UMBC CMSC435 F18 Project 1
*/

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <Eigen/Dense>
using Eigen::Vector3d;

// forward delcaration
class Surface;
class Test;

/**
 * class for storing info about the  material properties of a Surface
 */
class Material {
    friend class SettingsNFF;
    friend class RayTracer;
    friend class Test;
    private:
        Vector3d color;
        double Kd;        // diffuse component
        double Ks;        // specular component
        double shine;     // Pong cosing power for highlights
        double T;         // Transmittance (fraction of contribution of the transmitting ray)
        double refIndx;   // refraction idex
};

/**
 * class for parsing and storing info from an NFF file
 */
class SettingsNFF {
    friend class RayTracer;
    friend class Test;
    public:
        SettingsNFF();
        ~SettingsNFF();
        int readFile(std::string fname);
        friend std::ostream &operator<<(std::ostream &sout, const SettingsNFF &nff);

    private:
        static void printVector3d(std::ostream &sout, Vector3d vect, std::string label);

        Vector3d b_color;         // background color
        Vector3d v_from;
        Vector3d v_at;
        Vector3d v_up;
        double v_angle;
        // distance of plane in front of camera for which things closer to the camera
        // wont' be rendered:
        double v_hither;          // don't render things closer than this distance away
        Eigen::Vector2i v_resolution;     // x resolution, y resolution
        std::vector<Surface*> surfaces;   // all surfaces in scene
        std::vector<Material> materials;  // materials in nff file
};
#include "Surfaces.h"
#endif
