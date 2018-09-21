#ifndef _SETTINGSNFF_H_
#define _SETTINGSNFF_H_
/* File: SettingsNFF.h
    Dan Engbert - UMBC CMSC435 F18 Project 1
*/
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <Eigen/Dense>
#include "Surface.h"
#include "Polygon.h"
#include "Sphere.h"

using Eigen::Vector3d;

class SettingsNFF {
    public:
        SettingsNFF();
        int readFile(std::string fname);
        friend std::ostream &operator<<(std::ostream &sout, const SettingsNFF &nff);
        void reset();

    private:
        friend class RayTracer;
        static void printVector3d(std::ostream &sout, Vector3d vect, std::string label);

        Vector3d b_color;         // background color
        Vector3d v_from;
        Vector3d v_at;
        Vector3d v_up;
        double v_angle;
        // distance of plane in front of camera for which things closer to the camera
        // wont' be rendered:
        double v_hither;                 // don't render things closer than this distance away
        Eigen::Vector2i v_resolution;    // x resolution, y resolution
        std::vector<Surface*> surfaces;   // list of objects in scene
};
#endif
