#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "Tracer.h"
#include "Rasterizer.h"
#include "Test.h"
using Eigen::Vector3d;

using namespace std;

void runRasterizer(std::string filename);

// my driver
int main(int argc, char *argv[]) {
    /*
    // test my code
    Test test;
    test.testAll();
    return 0;
    */

    // parse args
    if (argc != 2) {
        std::cout << "expected 1 arg" << endl;
        exit(1);
    }
    string filename = argv[1];
    runRasterizer(filename);

    return 0;
}

/**
 * create the image of the nff file using rasterization
 */
void runRasterizer(std::string filename) {
    Rasterizer rast = Rasterizer(filename);
    rast.render("out.ppm", true);
}
