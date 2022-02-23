#include <Eigen/Dense>
#include <iostream>
#include "Test.h"
#include "Tracer.h"
#include "Rasterizer.h"

using Eigen::Vector3d;
using std::cout;
using std::endl;


// TODO: look into using gtest friends
//  https://github.com/google/googletest/blob/main/docs/reference/testing.md#friend_test-friend_test

/**
 * test methods in Rasterizer
 */
int Test::testRasterizer(std::string filename) {
    Rasterizer rast = Rasterizer(filename);
    Vector3d points[3] = {Vector3d(0,0,0), Vector3d(0,0,0), Vector3d(0,0,0)};
    Triangle tri = Triangle(points);

    // set image points
    tri.imgPoints[0] = Vector3d(0,0,0);
    tri.imgPoints[1] = Vector3d(4,0,0);
    tri.imgPoints[2] = Vector3d(0,4,0);

    int minX = -1, minY = -1;
    int maxX = 5, maxY = 5;

    //int x = 1;
    //int y = 6;
    //Fragment *frag = rast.getFrag(x, y, tri);
    //printf("is (%d, %d) in triangle: %d\n", x, y, (frag != NULL));

    // TODO: add some assertions that make sense or replace this with another unit test...
    for (int y=minY; y<=maxY; y++) {
        for (int x=minX; x<=maxX; x++) {
            Fragment *frag = rast.getFrag(x, y, tri);
            printf("is (%d, %d) in triangle: %d\n", x, y, (frag != NULL));
        }
    }
    return 0;
}
