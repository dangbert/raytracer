#include <Eigen/Dense>
#include <iostream>
#include "Test.h"
#include "Tracer.h"
#include "Rasterizer.h"

using namespace std;
using Eigen::Vector3d;

////////////////////////////
///// Test class: //////
////////////////////////////

int Test::testAll() {
    int res1 = testPolygons();
    int res2 = testTriangles();

    if (res1 == 0 && res2 == 0)
        cout << "\n\npassed ALL tests!" << endl;

    return 0;
}

/*
 * test that Polygon::intersectValues() works
 */
int Test::testPolygons() {
    cout << "\n---TESTING Polygon class:---" << endl;
    bool debug = false;
    vector<Vector3d> vertices;
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
    if (testOnePolygon(poly, ray, 5.5, expd, ++count, debug) != 0)
        return -1;

    expd = (ray.eye - Vector3d(2,1,5)).norm();
    if (testOnePolygon(poly, ray, 5.4, expd, ++count, debug) != 0)
        return -1;

    expd = (ray.eye - Vector3d(2,1,5)).norm();
    if (testOnePolygon(poly, ray, -1, expd, ++count, debug) != 0)
        return -1;

    cout << "[[[[POLYGON TESTS COMPLETE]]]]\n" << endl;
    delete matr;
    return 0;
}

int Test::testOnePolygon(Polygon poly, Ray ray, double hither, double expd, int testNum, bool debug) {
    if (debug)
        cout << "testing intersection of: " << ray << " and " << poly << endl;
    HitRecord hit = poly.intersect(ray, hither, -1, debug);
    cout << "test " << testNum << ": res = " << hit.dist << ", expd = " << expd << endl;
    if (hit.dist == expd) {
        cout << "[[test" << testNum << " PASSED]]\n" << endl;
        return 0;
    }
    else {
        cout << "[[test" << testNum << " FAILED]]\n" << endl;
        return -1;
    }
}

int Test::testOneTriangle(Triangle tri, Ray ray, double expd, int testNum, bool debug) {
    HitRecord hit = tri.intersect(ray, 0, -1, debug);
    cout << "test " << testNum << ": res = " << hit.dist << ", expd = " << expd << endl;
    if (hit.dist == expd) {
        cout << "[[test" << testNum << " PASSED]]\n" << endl;
        return 0;
    }
    cout << "[[test" << testNum << " FAILED]]\n" << endl;
    return -1;
}

/*
 * test that Triangle::intersectValues() works
 */
int Test::testTriangles() {
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
    return 0;
}

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

    for (int y=minY; y<=maxY; y++) {
        for (int x=minX; x<=maxX; x++) {
            Fragment *frag = rast.getFrag(x, y, tri);
            printf("is (%d, %d) in triangle: %d\n", x, y, (frag != NULL));
        }
    }
    return 0;
}
