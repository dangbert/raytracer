#include "Settings.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <iterator>
#include <assert.h>
using namespace std;
using Eigen::Vector3d;

////////////////////////////
//// SettingsNFF class: ////
////////////////////////////
SettingsNFF::SettingsNFF() {
    // default background color is 0,0,0
    b_color = Vector3d(0,0,0);
    v_from = Vector3d(0,0,0);
    v_at = Vector3d(0,0,0);
    v_up = Vector3d(0,0,0);
    v_angle = 0;
    v_hither = -1;  // -1 indicates that hither isn't set
    v_resolution = Eigen::Vector2i(0,0);
}

/*
 * destruct this object
 */
SettingsNFF::~SettingsNFF() {
    for (unsigned int i=0; i<surfaces.size(); i++) {
        delete surfaces[i];
    }
    surfaces.clear();
    for (unsigned int i=0; i<materials.size(); i++) {
        delete materials[i];
    }
    materials.clear();
}

/**
 * Read an NFF format file
 * (ignores things that are not yet implement in this class)
 */
int SettingsNFF::readFile(std::string fname) {
    if (surfaces.size() != 0) {
        cout << "this object has already read a file" << endl;
        return -1;
    }
    std::string line;
    std::string mode = "";  // for sections with multiple lines
    int p_remaining = 0;    // number of vertices remaining (when mode == "p")
    ifstream f(fname);
    std::vector<Vector3d> cur_vertices;    // vertices for current polygon

    while (std::getline(f, line)) {
        // split line on space delimeter to vector of strings
        std::istringstream iss1(line);
        std::istream_iterator<std::string> begin(iss1);
        std::istream_iterator<std::string> end;
        std::vector<std::string> tokens(begin, end);

        std::istringstream iss(line); // string stream to use below
        std::string tmp;
        // NOTE: all one line sections must come first in this if/else if section of code
        // background color (b %g %g %g)
        if (tokens.at(0) == "b") {
            mode = "b";
            if (!(iss >> tmp >> b_color[0] >> b_color[1] >> b_color[2])) { return -1; } // error
        }
        // fill color and shading parameters
        // these material properties apply to all the subsequent surfaces
        // (until a newer "f" line appears in the file)
        else if (tokens.at(0) == "f") {
            mode = "f";
            // "f" red green blue Kd Ks Shine T index_of_refraction
            // f %g %g %g %g %g %g %g %g
            Material *matr = new Material();
            if (!(iss >> tmp >> matr->color[0] >> matr->color[1] >> matr->color[2])) { return -1; }
            if (!(iss >> matr->Kd >> matr->Ks >> matr->shine >> matr->T >> matr->refIndx)) { return -1; }
            materials.push_back(matr);
        }
        else if (tokens.at(0) == "s") {
            // "s" center.x center.y center.z radius
            // s %g %g %g %g
            mode = "f";
            Vector3d center;
            double radius;
            if (!(iss >> tmp >> center[0] >> center[1] >> center[2] >> radius)) { return -1; }
            surfaces.push_back(new Sphere(materials.back(), center, radius));
        }
        else if (tokens.at(0) == "p" || mode == "p") {
            // "p" total_vertices
            // vert1.x vert1.y vert1.z
            // [etc. for total_vertices]
            mode = "p";
            if (tokens.at(0) == "p") {
                // store number of vertices to process
                if (!(iss >> tmp >> p_remaining)) { return -1; }
                cur_vertices.clear();
                continue; // advance to next line of file
            }
            Vector3d vert; // vertex
            // TODO: instead of this weird "mode" thing just do get line p_remaining times right here.
            if (!(iss >> vert[0] >> vert[1] >> vert[2])) { return -1; }
            cur_vertices.push_back(vert);

            if (--p_remaining == 0) {
                mode = ""; // "p" mode is over
                // create Polygon object using these vertices and the current fill color
                surfaces.push_back(new Polygon(materials.back(), cur_vertices));
                cur_vertices.clear();
            }
        }
        else if (tokens.at(0) == "pp") {
            // "pp" total_vertices
            // vert1.x vert1.y vert1.z norm1.x norm1.y norm1.z
            // [etc. for total_vertices]
            // store number of vertices to process
            if (!(iss >> tmp >> p_remaining)) { return -1; }
            cur_vertices.clear();
            std::vector<Vector3d> cur_normals;  // normals for polygon patch
            for (int i=0; i<p_remaining; i++) {
                if (! std::getline(f, line))
                    return -1;
                std::istringstream iss2(line);
                Vector3d vert, norm; // vertex, normal of vertex
                if (!(iss2 >> vert[0] >> vert[1] >> vert[2])) { return -1; }
                if (!(iss2 >> norm[0] >> norm[1] >> norm[2])) { return -1; }
                cur_vertices.push_back(vert);
                cur_normals.push_back(norm);
            }
            surfaces.push_back(new PolygonPatch(materials.back(), cur_vertices, cur_normals));
        }
        // viewing position
        else if (tokens.at(0) == "v" || mode == "v") {
            /*
            v
            from %g %g %g
            at %g %g %g
            up %g %g %g
            angle %g
            hither %g
            resolution %d %d
            */
            mode = "v";

            if (tokens.at(0) == "from") {
                if (!(iss >> tmp >> v_from[0] >> v_from[1] >> v_from[2])) { return -1; } // error
            }
            if (tokens.at(0) == "at") {
                if (!(iss >> tmp >> v_at[0] >> v_at[1] >> v_at[2])) { return -1; }
            }
            if (tokens.at(0) == "up") {
                if (!(iss >> tmp >> v_up[0] >> v_up[1] >> v_up[2])) { return -1; }
            }
            if (tokens.at(0) == "angle") {
                if (!(iss >> tmp >> v_angle)) { return -1; }
            }
            if (tokens.at(0) == "hither") {
                if (!(iss >> tmp >> v_hither)) { return -1; }
            }
            if (tokens.at(0) == "resolution") {
                if (!(iss >> tmp >> v_resolution[0] >> v_resolution[1])) { return -1; }
            }

        }

    }
    return 0; // success
}

// helper function for operator<<
void SettingsNFF::printVector3d(std::ostream &sout, Vector3d vect, std::string label) {
    sout << label << ": ";
    for (int i=0; i<3; i++) {
        sout << vect[i] << " ";
    }
    sout << endl;
}

// print out the NFF settings stored in this object for debugging
std::ostream& operator<<(std::ostream &sout, const SettingsNFF &nff) {
    sout << "\n---NFF Settings---\n";
    SettingsNFF::printVector3d(sout, nff.b_color, "b_color");
    SettingsNFF::printVector3d(sout, nff.v_from, "v_from");
    SettingsNFF::printVector3d(sout, nff.v_at, "v_at");
    SettingsNFF::printVector3d(sout, nff.v_up, "v_up");
    sout << "v_angle: " << nff.v_angle << endl;
    sout << "v_hither: " << nff.v_hither << endl;
    // TODO: consider doing above prints like this as well rather than using a helper function
    sout << "v_resolution: " << nff.v_resolution[0] << " " << nff.v_resolution[1] << endl;

    // get count of each type of surface and print the first occurence of each
    sout << nff.surfaces.size() << " surfaces total" << endl;
    int polyCount = 0;
    int polyPatchCount = 0;
    int sphereCount = 0;
    for (unsigned int i=0; i<nff.surfaces.size(); i++) {
        if (dynamic_cast<PolygonPatch*>(nff.surfaces[i]) != NULL) {
            if (polyPatchCount == 0) cout << *dynamic_cast<PolygonPatch*>(nff.surfaces[i]) << endl;
            polyPatchCount++;
        }
        else if (dynamic_cast<Polygon*>(nff.surfaces[i]) != NULL) {
            if (polyCount == 0) cout << *dynamic_cast<Polygon*>(nff.surfaces[i]) << endl;
            polyCount++;
        }
        else if (dynamic_cast<Sphere*>(nff.surfaces[i]) != NULL) {
            if (sphereCount == 0) cout << *dynamic_cast<Sphere*>(nff.surfaces[i]) << endl;
            sphereCount++;
        }
    }
    sout << "(" << polyCount << " polygons)" << endl;
    sout << "(" << polyPatchCount << " polygon patches)" << endl;
    sout << "(" << sphereCount << " spheres)" << endl;

    // print materials
    sout << "\nALL MATERIALS:" << endl;
    for (unsigned int i=0; i<nff.materials.size(); i++) {
        sout << *(nff.materials[i]) << endl;
    }
    sout << "------------------\n";
    return sout;
}

////////////////////////////
////// Material class: /////
////////////////////////////
// print out the NFF settings stored in this object for debugging
std::ostream &operator<<(std::ostream &sout, const Material &matr) {
    sout << "Material:" << std::endl;
    sout << "  color: (" << matr.color[0] << ", " << matr.color[1] << ", " <<  matr.color[2] << ")" << endl;
    sout << "  Kd=" << matr.Kd << ", ks=" << matr.Ks << endl;
    sout << "  shine=" << matr.shine << ", T=" << matr.T << ", refIndx=" << matr.refIndx << endl;
    return sout;
}
