#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "RayTracer.h"
using Eigen::Vector3d;

using namespace std;

void tests(SettingsNFF nff);
void testTriangles();

// my driver
int main(int argc, char *argv[]) {
    //testTriangles();
    //return 0;

    if (argc != 2) {
        std::cout << "expected 1 arg" << endl;
        exit(1);
    }
    SettingsNFF nff = SettingsNFF();
    cout << nff << endl;
    string filename = argv[1];
    int status = nff.readFile(filename);
    cout << "status = " << status << endl;
    cout << nff << endl;


    // create Ray Tracer using these settings
    RayTracer rayT = RayTracer(nff);

    // test my code
    //tests(nff);


    rayT.render("out.ppm");

    return 0;
}

void tests(SettingsNFF nff) {
    RayTracer rayT = RayTracer(nff);

    cout << "poly1:" << endl;
    vector<Vector3d> vertices;
    vertices.push_back(Vector3d(-1, -1, 1));
    vertices.push_back(Vector3d(-1, -0.5, 0.5));
    vertices.push_back(Vector3d(-0.5, -1, 0.5));
    Polygon poly1 = Polygon(vertices);
    cout << poly1 << endl;
    poly1.printTriangles();

    // test creation of triangle fan
    cout << "poly2:" << endl;
    vertices.clear();
    vertices.push_back(Vector3d(0,0,5));
    vertices.push_back(Vector3d(3,0,5));
    vertices.push_back(Vector3d(4,1,5));
    vertices.push_back(Vector3d(3,2,5));
    vertices.push_back(Vector3d(1.5,3,5));
    vertices.push_back(Vector3d(0,1,5));
    Polygon poly2 = Polygon(vertices);
    cout << poly2 << endl;
    poly2.printTriangles();
    vector<Triangle> tris = poly2.getTriangles();



    // test triangle and ray intersections
    //Ray r = Ray(Vector3d(2,1,0), Vector3d(2,1,5));
    //Triangle tri = Triangle(Vector3d(1,2,2), Vector3d(3,3,3), Vector3d(4,5,4));
    //Ray r = Ray(Vector3d(0,0,0), Vector3d(2.25,3,2.75));
    //Triangle tri = Triangle(Vector3d(4,2,4), Vector3d(5,-5,5), Vector3d(3,-2,6));
    Triangle tri = tris[0];
    //Ray r = Ray(Vector3d(0,0,-65), Vector3d(0,0,5));
    //Ray r = Ray(Vector3d(2,1,0), Vector3d(2,1,5));
    Ray r = Ray(Vector3d(2,1,0), Vector3d(2,0,5));

    cout << "\nTESTING Triangle intersection:" << endl;
    int res = tri.intersect(r);
    cout << "res = " << res << endl;

    cout << "\nTESTING Polygon intersection:" << endl;
    cout << "intersecting " << poly2 << " with " << r << endl;
    res = poly2.intersect(r);
    cout << "res = " << res << endl;
}

/*
 * test that Triangle::intersectValues() works
 */
void testTriangles() {
    int count = 0; // number of tests performed so far
    bool debug = false;
    cout << "TESTING Triangle class:" << endl;
    Triangle tri = Triangle(Vector3d(0,0,5), Vector3d(4,0,5), Vector3d(5,2,5));
    Ray ray = Ray(Vector3d(2,0,0), Vector3d(0,0,5));
    double vals[] = {-1,-1,-1, -1};

    // shoot ray at first vertex (from origin)
    ray = Ray(Vector3d(0,0,0), Vector3d(0,0,5));
    tri.intersectValues(ray, vals, debug);
    double expd = (ray.eye - ray.dir).norm();   // expected
    cout << "\texpd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "test" << ++count << " passed" << endl;

    // hit second vertex with ray
    ray = Ray(Vector3d(0,0,0), Vector3d(4,0,5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "\texpd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "test" << ++count << " passed" << endl;

    // hit the third vertex (from origin)
    ray = Ray(Vector3d(0,0,0), Vector3d(5,2,5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "\texpd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "test" << ++count << " passed" << endl;

    // shoot ray at third vertex (from opposite side of origin)
    ray = Ray(Vector3d(-5,-2,-5), Vector3d(5,2,5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "\texpd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "test" << ++count << " passed" << endl;

    // hit a point along bottom edge of triangle
    // this ray will pass through (2,0,5) just like a ray from (0,0,0) would
    ray = Ray(Vector3d(-2,0,-5), Vector3d(2,0,5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "\texpd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "test" << ++count << " passed" << endl;

    // hit a point along top edge of triangle (from origin)
    ray = Ray(Vector3d(0,0,0), Vector3d(1, (double) 2/5, 5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "\texpd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "test" << ++count << " passed" << endl;

    // hit a point in the middle of the triangle
    ray = Ray(Vector3d(0,0,0), Vector3d(4, 1, 5));
    tri.intersectValues(ray, vals, debug);
    expd = (ray.eye - ray.dir).norm();   // expected
    cout << "\texpd = " << expd << endl;
    assert(vals[3] == expd);
    cout << "test" << ++count << " passed" << endl;
}

