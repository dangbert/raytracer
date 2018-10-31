#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <assert.h>
#include "io.h"
using Eigen::Vector3d;
using namespace std;

int doSmoothing(char* iName, char* oName, double lambda, double dt, int iterations);

/**
 * usage:
 *  ./smoothing input.obj output.obj lambda dt niterations
 */
int main(int argc, char *argv[]) {
    char *iName, *oName;
    double lambda, dt;
    int iterations;

    // read command line args
    if (argc != 6) {
        cout << "exactly 6 args expected" << endl;
        cout << "USAGE:\n\t./smoothing input.obj output.obj lambda dt niterations" << endl;
        exit(1);
    }
    iName = argv[1];
    oName = argv[2];
    sscanf(argv[3], "%lf", &lambda);
    sscanf(argv[4], "%lf", &dt);
    sscanf(argv[5], "%d", &iterations);

    doSmoothing(iName, oName, lambda, dt, iterations);
    return 0;
}

int doSmoothing(char* iName, char* oName, double lambda, double dt, int iterations) {
    std::vector<Eigen::Vector3d> pts; // mesh points
    std::vector<Tri> tris;            // triangles (store indices into pts)
    // read obj file
    if (!readObjFile(iName, pts, tris)) {
        printf("ERROR: unable to read file: %s\n", iName);
        return -1;
    }

    // do smoothing:
    std::vector<Eigen::Vector3d> lap;
    std::vector<double> m;
    for (int n=0; n<iterations; n++) {
        // do one pass of smoothing:

        // initialize accumulators
        for (int i=0; i<pts.size(); i++) {
            //lap[i] = Eigen::Vector3d(0,0,0);
            lap.push_back(Eigen::Vector3d(0.0,0.0,0.0));
            m.push_back(0);
        }

        // loop over triangles
        for (int i=0; i<tris.size(); i++) {
            Tri &t = tris[i];
            for (int c=0; c<3; c++) {
                // vertex c
                m[t.indices[c]] += 2;
                //                  += other vertices - 2 * current vertex
                lap[t.indices[c]] += pts[t.indices[(c+1)%3]] + pts[t.indices[(c+2)%3]] - 2 * pts[t.indices[c]];
            }
        }

        // iterate over ever vertex
        for (int i=0; i<pts.size(); i++) {
            pts[i] += lambda * dt * 1.0/m[i] * lap[i];
        }
    }


    writeObjFile(oName, pts, tris);
    return 0;
}
