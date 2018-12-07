#define cimg_display 0
#include "CImg.h"
#include <Eigen/Dense>
#include <cmath>
#include <iostream>
using namespace cimg_library;
using std::string;
using std::cout;
using std::endl;

/*
 * get the corresponding index in a 1-D array for (col, row) in image (width x height)
 */
inline int getIndex(int col, int row, int width, int height) {
    return col * height + row;
}

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
            int index = getIndex(i, j, input.width(), input.height());
            image[index][0] = lab(i, j, 0);
            image[index][1] = lab(i, j, 1);
            image[index][2] = lab(i, j, 2);
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
            int index = getIndex(i, j, output.width(), output.height());
            output(i, j, 0) = image[index][0];
            output(i, j, 1) = image[index][1];
            output(i, j, 2) = image[index][2];
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

/**
 * returns the energy at a specified pixel in an image
 *
 * @image: image of pixel values
 * @col: column index of target pixel
 * @row: row index of target pixel
 *
 */
double getEnergy(Eigen::Vector3d *image, int col, int row) {
    int width = image->cols();
    int height = image->rows();
    double energy = 0;
    /* edge cases */
    if (0 == col) {

    }
    else if (width-1 == col) {

    }
    else if (0 == row) {

    }
    else if (height-1 == row) {

    }

    /* normal case */
    /* indices in image of pixels 1 to the left, right, up, down of current pixel */
    int index =  getIndex(col,   row,   width, height);
    int indexL = getIndex(col-1, row,   width, height);
    int indexR = getIndex(col+1, row,   width, height);
    int indexU = getIndex(col,   row-1, width, height);
    int indexD = getIndex(col,   row+1, width, height);
    //energy = abs(image[


}
