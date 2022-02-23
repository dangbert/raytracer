#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "Tracer.h"
//#include "Test.h"
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

    rayT.render("out.ppm", 5, false);
    // render the image
    /*
    for (int i=0; i<5; i++) {
        string tmp = "";
        stringstream ss;
        ss << "out" << i << ".ppm";
        ss >> tmp;
        rayT.render(tmp, i, false);
    }
    */

    // create animation
    //rayT.animate("out/out/frame", false);
    return 0;
}
