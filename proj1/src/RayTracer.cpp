#include <iostream>
#include "RayTracer.h"
#include <assert.h>
#include <math.h>
using namespace std;

RayTracer::RayTracer(SettingsNFF nff) {
    this->nff = nff;
}

/**
 * renders the nff file for this object and writes the image to a file
 * filename:  name for output file (should end in .ppm)
 * debug:     whether or not to print out debug info for the render proces
 *
 * for this project, FOV is the whole image
 *   (including the edge of pixels on each side rather than their centers)
 *   and pixels are square
 */
void RayTracer::render(std::string filename, bool debug) {
    /*
     * NOTE:
     * world coordinates:  3D coordinate system (camera focal point at origin)
     * camera coordinates: 3D coordinate system where:
     *                     center of image is origin, image plane is on z=0
     * image coordinates:  (i,j) discrete location of pixel in image
     */
    cout << "\nrendering..." << endl;
    if (debug) {
        cout << this->nff << endl;
    }
    // vectors defining our world coordinates (origin is at nff.v_from)
    Vector3d w = (nff.v_from - nff.v_at).normalized();
    Vector3d u = nff.v_up.cross(w).normalized();
    Vector3d v = w.cross(u).normalized();

    // distance from origin to center of virtual image plane
    double d = (nff.v_at - nff.v_from).norm();
    // width (and height) of a pixel on the virtual image plane
    //     deltaX = real world width of plane / x resolution
    double deltaX = 2 * (d * tan(nff.v_angle/2 * M_PI/180)) / nff.v_resolution[0];

    // position (in camera coordinates) of CENTER of top left corner pixel
    // locations are offset from the center of the image (0,0,0) (in camera coordinates)
    //                 deltaX * (num pixels to move)
    double initX = 0 - deltaX * ((double) nff.v_resolution[0]/2 - (double) 1/2);
    double initY = 0 + deltaX * ((double) nff.v_resolution[1]/2 - (double) 1/2);

    if (debug) {
        printf("w: (%g, %g, %g)\n", w[0], w[1], w[2]);
        printf("u: (%g, %g, %g)\n", u[0], u[1], u[2]);
        printf("v: (%g, %g, %g)\n", v[0], v[1], v[2]);
        cout << "d: " << d << endl;
        cout << "deltaX: " << deltaX << endl;
        cout << "initX: " << initX << endl;
        cout << "initY: " << initY << endl << endl;
    }

    // image of pixel values
    const int HEIGHT = nff.v_resolution[1];
    const int WIDTH = nff.v_resolution[0];
    // allocate contiguous bytes for storing pixel color values
    unsigned char* pixels = new unsigned char[HEIGHT*WIDTH*3];

    // iterate over all pixels in the image ((0,0) is the top left corner pixel)
    for (int i=0; i<WIDTH; i++) {      // iterate over columns
        for (int j=0; j<HEIGHT; j++) { // iterate over rows
            // (i,j) is the current pixel (in image coordinates)
            // center of this pixel (in camera coordinates)
            double x = initX + deltaX*i;
            double y = initY - deltaX*j;
            double z = -(nff.v_from - nff.v_at).norm();
            // transform (center of this pixel) camera coordinates -> world coordinates
            Vector3d pos = x*u + y*v + z*w + nff.v_from;

            if (debug && i%5 == 0 && (j == 0 || j == 128 || j == 511)) {
                printf("(%d,%d) -> (%f, %f, %f)\n", i, j, pos[0], pos[1], pos[2]);
            }
            // construct ray from camera through center of pixel
            Ray ray = Ray(nff.v_from, pos-nff.v_from);
            // get the color for this pixel
            Vector3d color = trace(ray);
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
    cout << "image written to " << filename << endl;
}

/**
 * return the color of the first object hit by this ray
 * (returns the background color if no object is hit)
 */
Vector3d RayTracer::trace(Ray ray, bool debug) {
    // TODO: use hither
    int closest = -1;  // index of closest polygon intersected by this ray
    int dist = -1;     // distance (along the ray) to the intersection point
    for (unsigned int i=0; i<nff.polygons.size(); i++) {
        int res;
        res = nff.polygons[i].intersect(ray, debug);

        if (res != -1 && (dist == -1 || res < dist)) {
            // we had our first intersection, or found a closer intersection
            closest = i;
            dist = res;
        }
    }

    if (dist != -1) {
        // ray intersected with an object
        // TODO: store a unique color corresponding to each polygon (in it's class?)
        return nff.f_rgb;
    }
    else {
        return nff.b_color;
    }
}
