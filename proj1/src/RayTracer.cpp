#include <iostream>
#include "RayTracer.h"
#include <assert.h>
#include <math.h>
using namespace std;

RayTracer::RayTracer(SettingsNFF nff) {
    this->nff = nff;
}

/**
 * for fun
 */
void RayTracer::animate(std::string filename, bool debug) {
    int count = 0;
    /*
    for (double h=0; h<10; h+=0.1) {
        std::string tmp = "";
        std::stringstream ss;
        ss << filename << count << ".ppm";
        ss >> tmp;

        this->nff.v_hither = h;
        render(tmp, debug);
        count++;
    }
    */
    Vector3d w = (nff.v_from - nff.v_at).normalized();
    Vector3d u = nff.v_up.cross(w).normalized();
    Vector3d v = w.cross(u).normalized();

    Vector3d start = this->nff.v_from;
    for (double a=0; a<100; a+=1) {
        double angle = (2*M_PI) * (a/100);
        std::string tmp = "";
        std::stringstream ss;
        ss << filename << count << ".ppm";
        ss >> tmp;

        Vector3d axis = v;
        Vector3d c = this->nff.v_at; // center of rotation

        Vector3d point = start;
        Eigen::Affine3d A = Eigen::Translation3d(c) * Eigen::AngleAxisd(angle, axis) * Eigen::Translation3d(-c);
        //Eigen::Vector4d point4 = point.conservativeResize(4);
        //point4(3)=1;
        Eigen::Vector4d point4;
        point4[0] = point[0];
        point4[1] = point[1];
        point4[2] = point[2];
        point4[3] = 1;
        point4=(A.matrix() * point4);
        //point = point4.conservativeResize(3);

        point[0] = point4[0] / point4[3];
        point[1] = point4[1] / point4[3];
        point[2] = point4[2] / point4[3];


        this->nff.v_from = point;
        //this->nff._from = Eigen::AngleAxis<float> aa(angle, this->nff.v_from);

        // create frame
        render(tmp, debug);
        count++;
    }
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
     * world coordinates:  3D coordinate system (which polygons are defined in respect to)
     * camera coordinates: 3D coordinate system where:
     *                     origin is the focal point (v_from)
     *                     center of virtual image plane is (0,0,d)
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
    //w = Vector3d(0,0,-1);
    //u = Vector3d(1,0,0);
    //v = Vector3d(0,1,0);

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
    for (int j=0; j<HEIGHT; j++) {      // iterate over rows
        for (int i=0; i<WIDTH; i++) {   // iterate over columns
            // (i,j) is the current pixel (in image coordinates)
            // center of this pixel (in camera coordinates)
            double x = initX + deltaX*i;
            double y = initY - deltaX*j;
            double z = -(nff.v_from - nff.v_at).norm();
            // transform (center of this pixel) camera coordinates -> world coordinates
            Vector3d pos = x*u + y*v + z*w + nff.v_from;

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
    int closest = -1;  // index of closest polygon intersected by this ray
    int dist = -1;     // distance (along the ray) to the intersection point
    for (unsigned int i=0; i<nff.polygons.size(); i++) {
        int res = nff.polygons[i].intersect(ray, nff.v_hither, debug);

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
