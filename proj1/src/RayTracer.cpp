#include <iostream>
#include "RayTracer.h"
#include <assert.h>
#include <math.h>
using namespace std;

RayTracer::RayTracer(SettingsNFF nff) {
    this->nff = nff;
}

void RayTracer::render(std::string filename) {
    cout << "\nrendering" << endl;

    // TODO: store these as Ray objects?
    Eigen::Vector3d w = (nff.v_from - nff.v_at).normalized();
    Eigen::Vector3d u = nff.v_up.cross(w).normalized();
    Eigen::Vector3d v = w.cross(u).normalized();

    cout << "w: " << w << endl;
    cout << "u: " << u << endl;
    cout << "v: " << v << endl;

    double d = (nff.v_at - nff.v_from).norm();
    double deltaX = (tan(nff.v_angle/2 * M_PI/180) * d) / (nff.v_resolution[0]/2);

    cout << "d: " << d << endl;
    cout << "deltaX: " << deltaX << endl;


}
