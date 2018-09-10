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

class SettingsNFF {
    public:
        SettingsNFF();
        int readFile(std::string fname);
        friend std::ostream &operator<<(std::ostream &sout, const SettingsNFF &nff);
        void reset();

    private:
        friend class RayTracer;
        static void printVector3d(std::ostream &sout, Eigen::Vector3d vect, std::string label);

        Eigen::Vector3d b_color;         // background color
        Eigen::Vector3d v_from;
        Eigen::Vector3d v_at;
        Eigen::Vector3d v_up;
        double v_angle;
        double v_hither;
        Eigen::Vector2i v_resolution;    // x resolution, y resolution
        Eigen::Vector3d f_rgb;           // fill color
        // vector of polygons (a polygon is a vector of Eigen::Vector3d)
        std::vector< std::vector<Eigen::Vector3d> > polygons;
};
#endif
