#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "Boids.h"

using Eigen::Vector3d;
using namespace std;

/**
 * usage:
 *  ./fishtank sample.in
 */
int main(int argc, char *argv[]) {
    // read command line args
    if (argc < 2) {
        printf("Usage:\n./fishtank sample.in\n");
        exit(1);
    }

    std::string fname(argv[1]);
    Flock flock(fname);
    flock.simulate();

    return 0;
}
