#include "Boids.h"
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <stdio.h>

using Eigen::Vector3d;
using std::cout;
using std::endl;

////////////////////////////
/////// Boid class: ////////
////////////////////////////


////////////////////////////
/////// Flock class: ///////
////////////////////////////
Flock::Flock(std::string fname) {
    readFile(fname);

    // print what was read for debugging
    cout << "bSize " << bSize << endl;
    cout << "nRadius " << nRadius << endl;
    cout << "nNeigbors " << nNeigbors << endl;
    cout << "mass " << mass << endl;
    cout << "wCollision " << wCollision << endl;
    cout << "wCentering " << wCentering << endl;
    cout << "wVelocity " << wVelocity << endl;
    cout << "wHunger " << wHunger << endl;
    cout << "damping " << damping << endl;
    cout << "dt " << dt << endl;
    cout << "duration " << duration << endl;

    printf("\nboids size = %lu\n", boids.size());
    printf("\nfoods size = %lu\n", foods.size());
}

/**
 * parses the file with path @fname (stores the results in this object)
 * exits if file doesn't parse as expected
 */
void Flock::readFile(std::string fname) {
    std::string line;
    std::ifstream f(fname);
    int flockSize;
    int nFood;

    // reset class
    boids.clear();
    foods.clear();

    // read first line
    getLineHelper(f, line);
    sscanf(line.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
        &bSize, &nRadius, &nNeigbors, &mass, &wCollision, &wCentering,
        &wVelocity, &wHunger, &damping, &dt, &duration);

    // read all boids
    getLineHelper(f, line);
    sscanf(line.c_str(), "%d", &flockSize);
    // read next *flockSize* lines and create a boid for each
    for (int i=0; i<flockSize; i++) {
        getLineHelper(f, line);
        Boid boi;
        sscanf(line.c_str(), "[%lf,%lf,%lf] [%lf,%lf,%lf]", &boi.position[0], &boi.position[1],
                &boi.position[2], &boi.velocity[0], &boi.velocity[1], &boi.velocity[2]);
        boids.push_back(boi);
    }

    // read line with nFood
    getLineHelper(f, line);
    sscanf(line.c_str(), "%d", &nFood);
    // read all food
    for (int i=0; i<nFood; i++) {
        getLineHelper(f, line);
        Food food;
        sscanf(line.c_str(), "[%lf,%lf,%lf] [%lf,%lf,%lf] %lf", &food.position[0], &food.position[1],
                &food.position[2], &food.velocity[0], &food.velocity[1], &food.velocity[2], &food.time);
        foods.push_back(food);
    }
}

/**
 * helper function that reads the next line from @f into @line
 * (skips past blank lines)
 */
void Flock::getLineHelper(std::ifstream &f, std::string &line) {
    do {
        if (!std::getline(f, line)) {
            printf("Error: while reading file.\n");
            exit(1);
        }
    }
    while (line == "");
}

