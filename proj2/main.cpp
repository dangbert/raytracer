#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "Tracer.h"
#include "Test.h"
using Eigen::Vector3d;

using namespace std;

void testTriangles();
void testPolygons();

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

    // create Ray Tracer using this nff file
    RayTracer rayT = RayTracer(filename);

    // render the image
    rayT.render("out.ppm", false);

    // create animation
    //rayT.animate("out/out", false);
    return 0;
}
