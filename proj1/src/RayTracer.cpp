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
 *
 * filename: name for output file (should end in .ppm)
 */
void RayTracer::render(std::string filename, bool debug) {
    cout << "\nrendering" << endl;

    // TODO: store these as Ray objects?
    Eigen::Vector3d w = (nff.v_from - nff.v_at).normalized();
    Eigen::Vector3d u = nff.v_up.cross(w).normalized();
    Eigen::Vector3d v = w.cross(u).normalized();


    double d = (nff.v_at - nff.v_from).norm();
    // deltaX = real world width / pixel width
    cout << "d = " << d << endl;
    double deltaX = (d * tan(nff.v_angle/2 * M_PI/180)) / (nff.v_resolution[0]/2);

    // TODO: my w,u,v, deltaX are correct
    //       but initX, initY are wrong
    //       teacher values:
    //          left limit: -1.69647
    //          top limit: 1.69647
    //       mine are both:
    //          initX: -1.70057


    //deltaX = 1.0;
    Vector3d v_at = nff.v_at;
    Eigen::Vector2i v_resolution = nff.v_resolution;
    //v_resolution[0] = 8;
    //v_resolution[1] = 9;
    //v_at[0] = 20;
    //v_at[1] = 20.5;

    // position of CENTER of top left corner pixel (real world coordinates)
    //                       deltaX * (numPixels to move)
    //double initX = v_at[0] - deltaX * ((double) v_resolution[0]/2 + (double) 1/2);
    cout << "resolution is " << v_resolution[0] << "x" << v_resolution[1] << endl;
    double numPixelsX = ((double) v_resolution[0]/2 - (double) 1/2);
    double initX = v_at[0] - deltaX * numPixelsX;

    double numPixelsY = ((double) v_resolution[1]/2 - (double) 1/2);
    double initY = v_at[1] + deltaX * numPixelsY;

    cout << "numPixelsX = " << numPixelsX << endl;
    cout << "numPixelsY = " << numPixelsY << endl;

    //double initX = nff.v_at[0] - deltaX * ((double) nff.v_resolution[0]/2) + deltaX/2;
    //double initY = nff.v_at[1] - deltaX * ((double) nff.v_resolution[1]/2) + deltaX/2;

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
    // TODO: swap i and j here?
    for (int i=0; i<WIDTH; i++) {      // iterate over columns
        for (int j=0; j<HEIGHT; j++) { // iterate over rows
            // camera coordinates
            double x = initX + deltaX*i;
            double y = initY - deltaX*j;
            double z = -(nff.v_from - nff.v_at).norm();
            // position of center of pixel (i,j) in world space (where u,v,w define the axies)
            Eigen::Vector3d pos = x*u + y*v + z*w + nff.v_from;

            if (i%5 == 0 && (j == 0 || j == 128 || j == 511)) {
                // for debugging
                printf("(%d,%d) -> (%f, %f, %f)\n", i, j, pos[0], pos[1], pos[2]);
            }
            // construct ray from camera through center of pixel
            Ray ray = Ray(nff.v_from, pos-nff.v_from);
            // get the color for this pixel
            Vector3d color = trace(ray);
            //std::cout << "final color: " << color[0] << "," << color[1] << "," << color[2] << endl;
            // set color of pixel
            for (int k=0; k<3; k++) {
                pixels[j*(WIDTH*3) + (i*3) + k] = color[k] * 255;
            }
        }
    }
    // write image to file
    FILE *f = fopen(filename.c_str(), "wb");
    fprintf(f, "P6\n%d %d\n%d\n", WIDTH, HEIGHT, 255);
    fwrite(pixels, 1, HEIGHT*WIDTH*3, f);
    fclose(f);
    delete[] pixels;
}

/**
 * return the color of the first object hit by this ray
 * returns the background color if no object is hit
 */
Eigen::Vector3d RayTracer::trace(Ray ray, bool debug) {
    //std::cout << "in trace, size = " << nff.polygons.size() << endl;
    // TODO: finish this
    //std::vector< std::vector<Eigen::Vector3d> > polygons;
    // iterate over polygons

    int closest = -1;  // index of closest polygon intersected
    int dist = -1;     // distance (along the ray) to the intersection point
    for (unsigned int i=0; i<nff.polygons.size(); i++) {
        int res;
        if (debug)
            res = nff.polygons[i].intersect(ray, debug);
        else
            res = nff.polygons[i].intersect(ray);

        if (res != -1 && (dist == -1 || res < dist)) {
            closest = i;
            dist = res;
        }
    }
    if (dist == -1) {
        // nothing was hit by the ray
        return nff.b_color;
    }
    else {
        // object was hit by the ray
        // TODO: store a unique color corresponding to each polygon (in it's class?)
        return nff.f_rgb;
    }
}
