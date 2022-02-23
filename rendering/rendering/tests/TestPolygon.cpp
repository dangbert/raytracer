#include <Eigen/Dense>
#include <iostream>
#include "Tracer.h"

#include <gtest/gtest.h>

using Eigen::Vector3d;
using std::cout;
using std::endl;

/**
 * TODO: remove testNum param
 */
void testOnePolygon(Polygon poly, Ray ray, double hither, double expd, int testNum, bool debug) {
    if (debug)
        cout << "testing intersection of: " << ray << " and " << poly << endl;
    HitRecord hit = poly.intersect(ray, hither, -1, debug);
    cout << "test " << testNum << ": res = " << hit.dist << ", expd = " << expd << endl;
    EXPECT_EQ(hit.dist, expd);
}

/*
 * test that Polygon::intersectValues() works
 */
//int Test::testPolygons() {
TEST(TestPolygon, intersect) {
    cout << "\n---TESTING Polygon class:---" << endl;
    bool debug = false;
    std::vector<Vector3d> vertices;
    vertices.push_back(Vector3d(0,0,5));
    vertices.push_back(Vector3d(3,0,5));
    vertices.push_back(Vector3d(4,1,5));
    vertices.push_back(Vector3d(3,2,5));
    vertices.push_back(Vector3d(1.5,3,5));
    vertices.push_back(Vector3d(0,1,5));
    Material *matr = new Material();
    Polygon poly = Polygon(matr, vertices);
    int count = 0;
    double expd;
   

    cout << "Testing with:" << endl;
    cout << poly << endl;

    // test hither
    Ray ray = Ray(Vector3d(0,0,0), Vector3d(2,1,5));

    expd = -1;
    testOnePolygon(poly, ray, 5.5, expd, ++count, debug);

    expd = (ray.eye - Vector3d(2,1,5)).norm();
    testOnePolygon(poly, ray, 5.4, expd, ++count, debug);

    expd = (ray.eye - Vector3d(2,1,5)).norm();
    testOnePolygon(poly, ray, -1, expd, ++count, debug);

    cout << "[[[[POLYGON TESTS COMPLETE]]]]\n" << endl;
    delete matr;
}