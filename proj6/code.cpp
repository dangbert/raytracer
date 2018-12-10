#define cimg_display 0
#include "CImg.h"
#include <Eigen/Dense>
#include <cmath>
#include <iostream>
using namespace cimg_library;
using std::string;
using std::cout;
using std::endl;

/* function prototypes */
double getEnergy(Eigen::Vector3d *image, int col, int row, int width, int height);
double getDx(Eigen::Vector3d *image, int col, int row, int width, int height);
double getDy(Eigen::Vector3d *image, int col, int row, int width, int height);
inline int getIndex(int col, int row, int width, int height);

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
    /* one dimensional array of 3D vectors */
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

    /*
    I also highly recommend outputing your an image of your energy function
    mapped to greyscale values. I set each pixel to pow(energy(i,j) / max_energy, 1.0/3.0).
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



/*
 * get the corresponding index in a 1-D array for (col, row) in image (width x height)
 */
inline int getIndex(int col, int row, int width, int height) {
    return col * height + row;
}

/**
 * returns the energy at a specified pixel in an image
 *
 * @image: image of pixel values
 * @col: column index of target pixel
 * @row: row index of target pixel
 * @width: width of @image
 * @height: height of @image
 */
double getEnergy(Eigen::Vector3d *image, int col, int row, int width, int height) {
    return getDx(image, col, row, width, height) + getDy(image, col, row, width, height);
}

/**
 * get the x-derivatve of the pixel in the image at point (col, row)
 * @Image: image in LAB color space
 * @col: column index of target pixel
 * @row: row index of target pixel
 * @width: width of @image
 * @height: height of @image
 */
double getDx(Eigen::Vector3d *image, int col, int row, int width, int height) {
    /* edge cases */
    int divide = 1, index1, index2;
    if (0 == col) {
        index1 = getIndex(col, row, width, height);
        index2 = getIndex(col+1, row, width, height);
    }
    else if (width-1 == col) {
        index1 = getIndex(col-1, row, width, height);
        index2 = getIndex(col, row, width, height);
    }
    else {
        index1 = getIndex(col-1, row, width, height);
        index2 = getIndex(col+1, row, width, height);
        divide = 2.0;
    }
    // use component 0 because that is the "lightness" value
    return abs(image[index2][0] - image[index1][0]) / (double) divide;
}

/**
 *
 * get the y-derivatve of the pixel in the image at point (col, row)
 * @Image: image in LAB color space
 * @col: column index of target pixel
 * @row: row index of target pixel
 * @width: width of @image
 * @height: height of @image
 */
double getDy(Eigen::Vector3d *image, int col, int row, int width, int height) {
    /* edge cases */
    int divide = 1, index1, index2;
    if (0 == row) {
        index1 = getIndex(col, row, width, height);
        index2 = getIndex(col, row+1, width, height);
    }
    else if (height-1 == row) {
        index1 = getIndex(col, row-1, width, height);
        index2 = getIndex(col, row, width, height);
    }
    else {
        index1 = getIndex(col, row-1, width, height);
        index2 = getIndex(col, row+1, width, height);
        divide = 2.0;
    }
    // use component 0 because that is the "lightness" value
    return abs(image[index2][0] - image[index1][0]) / (double) divide;
}
