#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <assert.h>
#include "io.h"
using Eigen::Vector3d;
using namespace std;

/**
 * usage:
 *  ./smoothing input.obj output.obj lambda dt niterations
 */
int main(int argc, char *argv[]) {
    string iName, oName;
    double lambda, dt;
    int iterations;

    if (argc != 6) {
        cout << "exactly 6 args expected" << endl;
        cout << "USAGE:\n\t./smoothing input.obj output.obj lambda dt niterations" << endl;
        exit(1);
    }
    iName = std::string(argv[1]);
    oName = std::string(argv[2]);
    sscanf(argv[3], "%lf", &lambda);
    sscanf(argv[4], "%lf", &dt);
    sscanf(argv[5], "%d", &iterations);

    printf("read variables: '%s', '%s', %f, %f, %d\n", iName.c_str(), oName.c_str(), lambda, dt, iterations);


    return 0;
}
