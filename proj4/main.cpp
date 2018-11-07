#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <assert.h>
#include "io.h"
#include "LinearSolver/cghs.h"                  // for implicit integration

using Eigen::Vector3d;
using namespace std;

struct Operator {
  int n;
  double lambda, dt;
  std::vector<Tri> tris;
};

int doSmoothing(char* iName, char* oName, double lambda, double dt, int iterations);
int implicitIntegration(char* iName, char* oName, double lambda, double dt, int iterations);
void mult (const Operator &op, double *v, double *w);
std::vector<Eigen::Vector3d> laplacian(const std::vector<Eigen::Vector3d> &pts, const std::vector<Eigen::Vector3d> &tris);

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

    //doSmoothing(iName, oName, lambda, dt, iterations);
    implicitIntegration(iName, oName, lambda, dt, iterations);
    return 0;
}

/**
 * perform desired nubmer of iterations of Laplacian smoothing
 */
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
        for (unsigned int i=0; i<pts.size(); i++) {
            //lap[i] = Eigen::Vector3d(0,0,0);
            lap.push_back(Eigen::Vector3d(0.0,0.0,0.0));
            m.push_back(0);
        }

        // loop over triangles
        for (unsigned int i=0; i<tris.size(); i++) {
            Tri &t = tris[i];
            for (int c=0; c<3; c++) {
                // vertex c
                m[t.indices[c]] += 2;
                //                  += other vertices - 2 * current vertex
                lap[t.indices[c]] += pts[t.indices[(c+1)%3]] + pts[t.indices[(c+2)%3]] - 2 * pts[t.indices[c]];
            }
        }

        // iterate over ever vertex
        for (unsigned int i=0; i<pts.size(); i++) {
            pts[i] += lambda * dt * 1.0/m[i] * lap[i];
        }
    }


    writeObjFile(oName, pts, tris);
    return 0;
}

/**
 * alternative to doSmoothing() that uses implicit integration.
 * this function based on: https://www.csee.umbc.edu/~adamb/435/implicit.cpp
 */
int implicitIntegration(char* iName, char* oName, double lambda, double dt, int iterations) {

    struct Operator op;
    std::vector<Eigen::Vector3d> pts; // mesh points
    // read obj file
    if (!readObjFile(iName, pts, op.tris)) {
        printf("ERROR: unable to read file: %s\n", iName);
        return -1;
    }
    op.n = pts.size(); op.lambda = lambda; op.dt = dt; //op.tris = tris;

    double *b = new double[3*pts.size()];
    double *x = new double[3*pts.size()];

    for (unsigned int i=0; i<pts.size(); i++) {
        for (int c=0; c<3; c++) {
            b[3*i+c] = pts[i][c];
            x[3*i+c] = pts[i][c];
        }
    }
    cout << "there are " << pts.size() << " points\n";

    double eps = 10e-10; // TODO: get from command line
    cghs<Operator>(3*pts.size(), op, b, x, eps, true);

    for (unsigned int i=0; i<pts.size(); i++) {
        pts[i] = Eigen::Vector3d(x[3*i+0], x[3*i+1], x[3*i+2]);
    }
    writeObjFile(oName, pts, op.tris);

    delete[] b;
    delete[] x;
    return 0;
}

/**
 * Laplacian function
 */
std::vector<Eigen::Vector3d> laplacian(const std::vector<Eigen::Vector3d> &pts, const std::vector<Tri> &tris) {
    std::vector<Eigen::Vector3d> lap;

    // initialize accumulators
    for (unsigned int i=0; i<pts.size(); i++) {
        lap.push_back(Eigen::Vector3d(0.0,0.0,0.0));
    }

    // loop over triangles
    for (unsigned int i=0; i<tris.size(); i++) {
        const Tri &t = tris[i];
        for (int c=0; c<3; c++) {
            // vertex c
            //                  += other vertices - 2 * current vertex
            lap[t.indices[c]] += pts[t.indices[(c+1)%3]] + pts[t.indices[(c+2)%3]] - 2 * pts[t.indices[c]];
        }
    }
    return lap;
}

/**
 * this function based on: https://www.csee.umbc.edu/~adamb/435/implicit.cpp
 */
void mult (const Operator &op, double *v, double *w) {
  std::vector<Eigen::Vector3d> pts;
  pts.resize(op.n);
  for (unsigned int i=0; i<pts.size(); i++) {
      //printf("flag1 i=%d\n", i);
      pts[i] = Eigen::Vector3d(v[3*i+0], v[3*i+1], v[3*i+2]);
  }

  std::vector<Eigen::Vector3d> l = laplacian(pts, op.tris);
  for (unsigned int i=0; i<pts.size(); i++) {
    l[i] *= op.lambda*op.dt;
    w[3*i+0] = v[3*i+0] - l[i][0];
    w[3*i+1] = v[3*i+1] - l[i][1];
    w[3*i+2] = v[3*i+2] - l[i][2];
  }
}
