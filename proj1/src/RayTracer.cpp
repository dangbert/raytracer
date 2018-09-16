#include <iostream>
#include "RayTracer.h"
#include <assert.h>
#include <math.h>
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

    cout << "w: " << w << endl;
    cout << "u: " << u << endl;
    cout << "v: " << v << endl;
    cout << "d: " << d << endl;
    cout << "deltaX: " << deltaX << endl;
    cout << "initX: " << initX << endl;
    cout << "initY: " << initY << endl;

    // image of pixel values
    const int HEIGHT = nff.v_resolution[1];
    const int WIDTH = nff.v_resolution[0];
    // allocate contiguous bytes for storing pixel color values
    unsigned char* pixels = new unsigned char[HEIGHT*WIDTH*3];

    // iterate over all pixels in the image (0,0) is the top left corner pixel
    // TODO: create array/matrix of pixel colors
    for (int i=0; i<WIDTH; i++) {
        std::cout << "i=" << i << endl;
        for (int j=0; j<HEIGHT; j++) {
            // position of center of pixel (i,j)
            Eigen::Vector3d pos = Eigen::Vector3d(initX, initY, nff.v_at[2]);
            Ray ray = Ray(nff.v_from, pos);     // ray from camera through center of pixel
            Eigen::Vector3d color = trace(ray);
            // set color of pixel
            for (int k=0; k<3; k++) {
                pixels[j*(WIDTH*3) + (i*3) + k] = color[k] * 255;
            }
        }
    }
    // write image to file
    FILE *f = fopen("out.ppm","wb");
    fprintf(f, "P6\n%d %d\n%d\n", WIDTH, HEIGHT, 255);
    fwrite(pixels, 1, HEIGHT*WIDTH*3, f);
    fclose(f);
    delete[] pixels;
}

/**
 * return the color for the pixel corresponding to this ray
 */
Eigen::Vector3d RayTracer::trace(Ray ray) {
    //std::cout << "in trace, size = " << nff.polygons.size() << endl;
    // TODO: finish this
    //std::vector< std::vector<Eigen::Vector3d> > polygons;
    // iterate over polygons

    int closest = -1;  // index of polygon intersected
    int dist = -1;
    for (unsigned int i=0; i<nff.polygons.size(); i++) {
        int res = nff.polygons[i].intersect(ray);
        if ((dist == -1) || (res != -1 && res < dist)) {
            closest = i;
            dist = res;
        }
    }
    if (closest == -1) {
        return nff.b_color;
    }
    else {
        // TODO: store a unique color corresponding to each polygon (in it's class?)
        return nff.f_rgb;
    }
}
