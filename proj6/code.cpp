#define cimg_display 0
#include "CImg.h"
#include <Eigen/Dense>
#include <cmath>
#include <iostream>
#include <limits>
using namespace cimg_library;
using std::string;
using std::cout;
using std::endl;

/* function prototypes */
int getIndex(int col, int row, int width, int height);
Eigen::Vector3d *removeVerticalSeam(Eigen::Vector3d *image, int width, int height);

double getEnergy(Eigen::Vector3d *image, int col, int row, int width, int height);
double getDx(Eigen::Vector3d *image, int col, int row, int width, int height);
double getDy(Eigen::Vector3d *image, int col, int row, int width, int height);
void viewEnergy(Eigen::Vector3d *image, int width, int height, char* output_image, int* seam=NULL);

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

    /******************************************/
    /* do work here */
    /* display the energy of the original image */
    ///viewEnergy(image, input.width(), input.height(), output_image);

    /* remove seams */
    //int curWidth
    //int curHeight = input.height();
    for (int curWidth = input.width(); curWidth > output_width; curWidth--) {
        image = removeVerticalSeam(image, curWidth, input.height());
    }

    /******************************************/

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
int getIndex(int col, int row, int width, int height) {
    return col * height + row;
}

/*
 (greedy algorithm )
 look at a set of potential seams, and remove the most optimal seam from the image
 1. compute 2d array of energy (at each pixel) 
 2. compute 2d array of seam cost (at each pixel)
 3. start from bottom row and find the pixel with min seam cost, and trace
    a path up from there (choosing the min of the 3 options each time)
 4. remove that seam path
 */
Eigen::Vector3d *removeVerticalSeam(Eigen::Vector3d *image, int width, int height) {
    /* populate energy matrix */
    double energy[width * height];
    double max = 0.0;
    for (unsigned int i=0; i<width; i++) {
        for (unsigned int j=0; j<height; j++) {
            int index = getIndex(i, j, width, height);
            energy[index] = getEnergy(image, i, j, width, height);
            max = std::max<double>(max, energy[index]);
        }
    }

    /* compute cumulative minimum energy (cost) */
    double *M = new double[width * height];
    // first row is trivial
    for (unsigned int i=0; i<width; i++) {
        int index = getIndex(i, 0, width, height);
        M[index] = energy[index];
    }

    // compute for rest of rows
    for (unsigned int col=1; col<width; col++) {
        for (unsigned int row=0; row<height; row++) {
            int index = getIndex(col, row, width, height);

            // compute the cost of the possible connections above (up to three)
            double C[3] = {std::numeric_limits<double>::max(),
                std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
            if (0 != col)
                C[0] = M[getIndex(col-1, row-1, width, height)];
            C[1] = M[getIndex(col, row-1, width, height)];
            if (width-1 != col)
                C[2] = M[getIndex(col+1, row-1, width, height)];

            // store the cumulative min energy for this pixel
            M[index] = energy[index] + std::min<double>(std::min<double>(C[0], C[1]), C[2]);
        }
    }

    /* find the cheapest connected path (the seam to remove) */
    int *seam = new int[height]; // stores selected col index for each row of image
    int leftCol = 0, rightCol = width-1; // range of columns to search
    for (unsigned int row=height-1; row!=0; row--) {
        seam[row] = leftCol;     // column with smallest M value in this row
        for (unsigned int col=leftCol; col<=rightCol; col++) {
            int index = getIndex(col, row, width, height);
            if (M[index] < M[getIndex(seam[row], col, width, height)])
                seam[row] = col;
        }
        // update leftCol and rightCol for next row's search
        leftCol = std::max(0, seam[row]-1);
        rightCol = std::min(width-1, seam[row]+1);
    }

    std::string fname = std::to_string(width) + ".jpg";
    char *outName = &fname[0u];
    viewEnergy(image, width, height, outName, seam);
    /* remove seam from image */
    Eigen::Vector3d *newImage = new Eigen::Vector3d[(width-1)*height];
    for (unsigned int row=0; row<height; row++) {
        double minCost = std::numeric_limits<double>::max();
        int offset = 0; // column offset (once seam location in row is passed)
        for (unsigned int col=0; col<width-1; col++) {
            if (col == seam[row]) {
                // we hit the pixel in the seam (use the offset to skip it)
                offset++;
            }
            for (unsigned int k=0; k<3; k++) {
                newImage[getIndex(col, row, width, height)][k] = image[getIndex(col+offset, row, width, height)][k];
            }
        }
    }
    delete[] image;
    return newImage;
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

/**
 * create an image showing the energy of the pixels in the provided image
 * and write it to a file
 *
 * @image: image of pixel values
 * @width: width of @image
 * @height: height of @image
 * @output_image: name of the file to output ("out-energy-" will be prepended to it)
 */
void viewEnergy(Eigen::Vector3d *image, int width, int height, char* output_image, int* seam) {
    string fname(output_image);
    if (seam == NULL)
        fname.insert(0, "out-energy-");
    else
        fname.insert(0, "out-seam-");
    /* populate energy matrix */
    double *energy = new double[width * height];
    double max = 0.0;
    for (unsigned int i=0; i<width; i++) {
        for (unsigned int j=0; j<height; j++) {
            int index = getIndex(i, j, width, height);
            energy[index] = getEnergy(image, i, j, width, height);
            max = std::max<double>(max, energy[index]);
        }
    }

    cout << "max energy is " << max << endl;
    /* write to output image */
    CImg<float> output(width, height, 1, 3);
    for (unsigned int i=0; i<output.width(); i++) {
        for (unsigned int j=0; j<output.height(); j++) {
            int index = getIndex(i, j, output.width(), output.height());
            output(i, j, 0) = 255.0*pow(energy[index]/max, 1/3.0);
            output(i, j, 1) = 255.0*pow(energy[index]/max, 1/3.0);
            output(i, j, 2) = 255.0*pow(energy[index]/max, 1/3.0);

            if (seam != NULL && i == seam[j]) {
                output(i, j, 0) = 255.0;
                output(i, j, 1) = 255.0;
                output(i, j, 2) = 0;
            }
        }
    }
    output.save_png(fname.c_str());
    delete[] energy;
}
