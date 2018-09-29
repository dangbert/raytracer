#include <Eigen/Dense>
#include <iostream>
#include "Test.h"
#include "Tracer.h"

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
    double res;

    cout << "Testing with:" << endl;
    cout << poly << endl;

    // test hither
    Ray ray = Ray(Vector3d(0,0,0), Vector3d(2,1,5));

    res = poly.intersect(ray, 5.5, true);
    cout << "test " << ++count << ": res = " << res << endl;
    assert(res == -1);
    cout << "[[test" << count << " passed]]\n" << endl;

    res = poly.intersect(ray, 5.4, false);
    cout << "test " << ++count << ": res = " << res << endl;
    assert(res == (ray.eye - Vector3d(2,1,5)).norm());
    cout << "[[test" << count << " passed]]\n" << endl;

    res = poly.intersect(ray, -1, false);
    cout << "test " << ++count << ": res = " << res << endl;
    cout << "res = " << res << endl;
    assert(res == (ray.eye - Vector3d(2,1,5)).norm());
    cout << "[[test" << count << " passed]]\n" << endl;

    cout << "[[[[POLYGON TESTS COMPLETE]]]]\n" << endl;
    delete matr;
    return 0;
}

/*
 * test that Triangle::intersectValues() works
 */
int Test::testTriangles() {
    int count = 0; // number of tests performed so far
    bool debug = false;
    cout << "\n---TESTING Triangle class:---" << endl;
    Triangle tri = Triangle(Vector3d(0,0,5), Vector3d(4,0,5), Vector3d(5,2,5));
    Ray ray = Ray(Vector3d(2,0,0), Vector3d(0,0,5));
    double vals[] = {-1,-1,-1, -1};

    // shoot ray at first vertex (from origin)
    ray = Ray(Vector3d(0,0,0), Vector3d(0,0,5));
    tri.intersectValues(ray, vals, debug);
    double expd = (ray.eye - ray.dir).norm();   // expected
    cout << "test " << ++count << ": res = " << vals[3] << ", expd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "[[test" << count << " passed]]\n" << endl;

    // hit second vertex with ray
    ray = Ray(Vector3d(0,0,0), Vector3d(4,0,5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "test " << ++count << ": res = " << vals[3] << ", expd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "[[test" << count << " passed]]\n" << endl;

    // hit the third vertex (from origin)
    ray = Ray(Vector3d(0,0,0), Vector3d(5,2,5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "test " << ++count << ": res = " << vals[3] << ", expd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "[[test" << count << " passed]]\n" << endl;

    // shoot ray at third vertex (from opposite side of origin)
    ray = Ray(Vector3d(-5,-2,-5), Vector3d(5,2,5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "test " << ++count << ": res = " << vals[3] << ", expd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "[[test" << count << " passed]]\n" << endl;

    // hit a point along bottom edge of triangle
    // this ray will pass through (2,0,5) just like a ray from (0,0,0) would
    ray = Ray(Vector3d(-2,0,-5), Vector3d(2,0,5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "test " << ++count << ": res = " << vals[3] << ", expd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "[[test" << count << " passed]]\n" << endl;

    // hit a point along top edge of triangle (from origin)
    ray = Ray(Vector3d(0,0,0), Vector3d(1, (double) 2/5, 5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "test " << ++count << ": res = " << vals[3] << ", expd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "[[test" << count << " passed]]\n" << endl;

    // hit a point in the middle of the triangle
    ray = Ray(Vector3d(0,0,0), Vector3d(4, 1, 5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "test " << ++count << ": res = " << vals[3] << ", expd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "[[test" << count << " passed]]\n" << endl;

    // hit a point in the middle of the triangle
    ray = Ray(Vector3d(0,0,0), Vector3d(3, 0.1, 5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "test " << ++count << ": res = " << vals[3] << ", expd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "[[test" << count << " passed]]\n" << endl;

    cout << "[[[[TRIANGLE TESTS COMPLETE]]]]\n" << endl;
    return 0;
}
