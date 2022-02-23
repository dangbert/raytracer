#include <Eigen/Dense>
#include <iostream>
#include "Tracer.h"

#include <gtest/gtest.h>

using Eigen::Vector3d;
using std::cout;
using std::endl;

// TODO: remove testNum param
void testOneTriangle(Triangle tri, Ray ray, double expd, int testNum, bool debug) {
    HitRecord hit = tri.intersect(ray, 0, -1, debug);
    cout << "test " << testNum << ": res = " << hit.dist << ", expd = " << expd << endl;
    EXPECT_EQ(hit.dist, expd);
}

/*
 * test that Triangle::intersectValues() works
 */
TEST(TestTriangle, intersect) {
    int count = 0; // number of tests performed so far
    bool debug = true;
    cout << "\n---TESTING Triangle class:---" << endl;
    Vector3d points[3] = {Vector3d(0,0,5), Vector3d(4,0,5), Vector3d(5,2,5)};
    Triangle tri = Triangle(points);
    Ray ray = Ray(Vector3d(2,0,0), Vector3d(0,0,5));

    // shoot ray at first vertex (from origin)
    ray = Ray(Vector3d(0,0,0), Vector3d(0,0,5));
    double expd = (ray.eye - ray.dir).norm();   // expected
    testOneTriangle(tri, ray, expd, ++count, debug);


    // hit second vertex with ray
    ray = Ray(Vector3d(0,0,0), Vector3d(4,0,5));
    expd = (ray.eye - ray.dir).norm();   // expected
    testOneTriangle(tri, ray, expd, ++count, debug);

    // hit the third vertex (from origin)
    ray = Ray(Vector3d(0,0,0), Vector3d(5,2,5));
    expd = (ray.eye - ray.dir).norm();   // expected
    testOneTriangle(tri, ray, expd, ++count, debug);

    // shoot ray at third vertex (from opposite side of origin)
    ray = Ray(Vector3d(-5,-2,-5), Vector3d(5,2,5));
    expd = (ray.eye - ray.dir).norm();   // expected
    testOneTriangle(tri, ray, expd, ++count, debug);


    // hit a point along bottom edge of triangle
    // this ray will pass through (2,0,5) just like a ray from (0,0,0) would
    ray = Ray(Vector3d(-2,0,-5), Vector3d(2,0,5));
    expd = (ray.eye - ray.dir).norm();   // expected
    testOneTriangle(tri, ray, expd, ++count, debug);

    // hit a point along top edge of triangle (from origin)
    ray = Ray(Vector3d(0,0,0), Vector3d(1, 2.0/5.0, 5));
    expd = (ray.eye - ray.dir).norm();   // expected
    testOneTriangle(tri, ray, expd, ++count, debug);

    // hit a point in the middle of the triangle
    ray = Ray(Vector3d(0,0,0), Vector3d(4, 1, 5));
    expd = (ray.eye - ray.dir).norm();   // expected
    testOneTriangle(tri, ray, expd, ++count, debug);

    // hit a point in the middle of the triangle
    ray = Ray(Vector3d(0,0,0), Vector3d(3, 0.1, 5));
    expd = (ray.eye - ray.dir).norm();   // expected
    testOneTriangle(tri, ray, expd, ++count, debug);
    cout << "[[[[TRIANGLE TESTS COMPLETE]]]]\n" << endl;
}