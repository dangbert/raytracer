#include <iostream>
#include "Rasterizer.h"
#include <assert.h>
#include <math.h>
using namespace std;

////////////////////////////
///// Rasterizer class: ////
////////////////////////////
/**
 * constructs a Rasterizer object
 * filename:  name of nff file to use
 */
Rasterizer::Rasterizer(std::string filename) {
    if (nff.readFile(filename) != 0) {
        cout << "error parsing " << filename << endl;
        exit(1);
    }
    cout << nff << endl;
}

/**
 * renders the nff file for this object and writes the image to a file
 * filename:  name for output file (should end in .ppm)
 * debug:     whether or not to print out debug info for the render proces
 */
void Rasterizer::render(std::string filename, bool debug) {
    cout << "\nrendering..." << endl;
    if (debug) {
        cout << nff << endl;
    }

    // vectors defining our world coordinates (origin is at nff.v_from)
    Vector3d eye = nff.v_from;
    Vector3d w = (nff.v_from - nff.v_at).normalized();
    Vector3d u = nff.v_up.cross(w).normalized();
    Vector3d v = w.cross(u).normalized();

    cout << "\nrasterizing..." << endl;
    Eigen::Matrix4d M = createMatrix(u, v, w, debug);
    return;


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
            /*
            Vector3d color = trace(ray, nff.v_hither, -1, bounces, debug);
            // set color of pixel
            for (int k=0; k<3; k++) {
                color[k] = min(max(color[k], 0.0), 1.0); // force value in range [0,1]
                pixels[j*(WIDTH*3) + (i*3) + k] = (int) (color[k] * 255);
            }
            */
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
 * returns the matrix needed to tranform vertices
 */
Eigen::Matrix4d Rasterizer::createMatrix(Vector3d u, Vector3d v, Vector3d w, bool debug) {
    double h =       tan(nff.v_angle / 2.0 * M_PI/180.0);
    double right =   h;
    double left =   -1.0 * right;
    double top =    right / (nff.v_resolution[0]/nff.v_resolution[1]);
    double bottom = left / (nff.v_resolution[0]/nff.v_resolution[1]);
    double near =   -1.0 *nff.v_hither;
    double far =    1000 * near;

    Vector3d eye = nff.v_from;
    int nx = nff.v_resolution[0];
    int ny = nff.v_resolution[1];

    Eigen::Matrix4d M_vp, M_per, M;
    // p. 144 in textbook:
    M_vp << nx/2.0, 0, 0, (nx-1)/2.0,
            0, ny/2.0, 0, (ny-1)/2.0,
            0, 0, 1, 0,
            0, 0, 0, 1;

    // p. 155 in textbook:
    M_per << (2.0*near)/(right-left), 0, (left+right)/(left-right), 0,
            0, (2.0*near)/(top-bottom), (bottom+top)/(bottom-top), 0,
            0, 0, (far+near)/(near-far), (2.0*far*near)/(far-near),
            0, 0, 1, 0;

    Eigen::MatrixXd M_cam; // dynamic resize matrix
    M_cam.resize(3,4);
    M_cam << u, v, w, eye;
    M_cam.conservativeResize(4, 4);
    M_cam.row(3) = Eigen::Vector4d(0,0,0,1);
    M_cam = M_cam.inverse();

    M = M_vp * M_per * M_cam;
    if (debug) {
        cout << "M_vp =\n" << M_vp << endl;
        cout << "M_per=\n" << M_per << endl;
        cout << "M_cam=\n" << M_cam << endl;
        cout << "M\n" << M << endl;
    }
    return M;
}
