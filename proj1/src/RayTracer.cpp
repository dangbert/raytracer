#include <iostream>
#include "RayTracer.h"
#include <assert.h>
#include <math.h>
#include "Ray.h"
using namespace std;

RayTracer::RayTracer(SettingsNFF nff) {
    this->nff = nff;
}

/**
 *
 * for this project, FOV is the whole image
 *   (including the edge of pixels on each side rather than their centers)
 *   and pixels are square
 */
void RayTracer::render(std::string filename) {
    cout << "\nrendering" << endl;

    // TODO: store these as Ray objects?
    Eigen::Vector3d w = (nff.v_from - nff.v_at).normalized();
    Eigen::Vector3d u = nff.v_up.cross(w).normalized();
    Eigen::Vector3d v = w.cross(u).normalized();


    double d = (nff.v_at - nff.v_from).norm();
    // deltaX = real world width / pixel width
    double deltaX = (d * tan(nff.v_angle/2 * M_PI/180)) / (nff.v_resolution[0]/2);

    // position of center of top left corner pixel (real world coordinates)
    double initX = nff.v_at[0] - deltaX * ((double) nff.v_resolution[0]/2) + deltaX/2;
    double initY = nff.v_at[1] - deltaX * ((double) nff.v_resolution[1]/2) + deltaX/2;

    // iterate over all pixels in the image (0,0) is the top left corner pixel
    for (int i=0; i<nff.v_resolution[0]; i++) {
        for (int j=0; j<nff.v_resolution[1]; j++) {
            // position of center of pixel (i,j)
            Eigen::Vector3d pos = Eigen::Vector3d(initX, initY, nff.v_at[2]);
            // ray from camera through center of pixel
            Ray ray = Ray(nff.v_from, pos);

        }

    }



    cout << "w: " << w << endl;
    cout << "u: " << u << endl;
    cout << "v: " << v << endl;
    cout << "d: " << d << endl;
    cout << "deltaX: " << deltaX << endl;
}
