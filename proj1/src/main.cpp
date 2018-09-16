#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "RayTracer.h"

using namespace std;

void tests(SettingsNFF nff);

// my driver
int main(int argc, char *argv[]) {
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
    RayTracer ray = RayTracer(nff);

    // test my code
    tests(nff);


    //ray.render("out.ppm");

    return 0;
}

void tests(SettingsNFF nff) {
    RayTracer rayT = RayTracer(nff);
    vector<Eigen::Vector3d> vertices;
    vertices.push_back(Eigen::Vector3d(-1, -1, 1));
    vertices.push_back(Eigen::Vector3d(-1, -0.5, 0.5));
    vertices.push_back(Eigen::Vector3d(-0.5, -1, 0.5));

    Polygon poly = Polygon(vertices);
    cout << poly << endl;

    poly.printTriangles();

}
