#define cimg_display 0
#include "CImg.h"
#include <Eigen/Dense>
using namespace cimg_library;
using std::string;

/**
 *
 * USAGE:
 * ./seamcarving <input_image> <output_image> <output_width> <output_height>
 */
int main(int argc, char *argv[]) {
    char* input_image = argv[1];
    char* output_image = argv[2];
    int output_width = atoi(argv[3]);
    int output_height = atoi(argv[4]);

    CImg<double> input(argv[1]);
    CImg<double> lab = input.RGBtoLab();

    /* convert input image to LAB color space */
    Eigen::Vector3d *image = new Eigen::Vector3d[input.width()*input.height()];
    for (unsigned int i=0; i<input.width(); i++) {
        for (unsigned int j=0; j<input.height(); j++) {
            image[i*input.height()+j][0] = lab(i, j, 0);
            image[i*input.height()+j][1] = lab(i, j, 1);
            image[i*input.height()+j][2] = lab(i, j, 2);
        }
    }

    /* do work here */
    /*
     (greedy algorithm )
     look at a set of potential seams, and remove the most optimal seam from the image
     1. compute 2d array of energy (at each pixel) 
     2. compute 2d array of seam cost (at each pixel)
     3. start from bottom row and find the pixel with min seam cost, and trace
        a path up from there (choosing the min of the 3 options each time)
     4. remove that seam path
     */


    /* output image to file */
    CImg<double> output(output_width, output_height, input.depth(), input.spectrum(), 0);
    for (unsigned int i=0; i<output.width(); i++) {
        for (unsigned int j=0; j<output.height(); j++) {
            output(i, j, 0) = image[i*output.height()+j][0];
            output(i, j, 1) = image[i*output.height()+j][1];
            output(i, j, 2) = image[i*output.height()+j][2];
        }
    }

    CImg<double> rgb = output.LabtoRGB();
    if (strstr(output_image, ".png") || strstr(output_image, ".PNG"))
        rgb.save_png(output_image);
    else if (strstr(output_image, ".jpg") || strstr(output_image, ".JPG"))
        rgb.save_jpeg(output_image);

    delete [] image;
    return 0;
}
