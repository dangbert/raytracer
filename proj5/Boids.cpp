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
}

int Flock::readFile(std::string fname) {
    std::istringstream iss;
    std::string line;
    std::ifstream f(fname);
    int flockSize;
    int nFood;

    // reset class
    boids.clear();
    foods.clear();

    // read first line
    if (!std::getline(f, line)) goto handleErr;
    iss = std::istringstream(line);
    if (!(iss >> bSize >> nRadius >> nNeigbors >> mass >> wCollision >> wCentering
                >> wVelocity >> wHunger >> damping >> dt >> duration))
        goto handleErr;

    // read all boids
    if (!std::getline(f, line)) goto handleErr;
    sscanf(line.c_str(), "%s", &flockSize);
    printf("flocksize is %s\n", flockSize);
    // read next *flockSize* lines and create a boid for each
    for (int i=0; i<flockSize; i++) {
        if (!std::getline(f, line)) goto handleErr;
        Boid boi;
        sscanf(line.c_str(), "[%lf %lf %lf] [%lf %lf %lf]", boi.position[0], boi.position[1],
                boi.position[2], boi.velocity[0], boi.velocity[1], boi.velocity[2]);
        boids.push_back(boi);
    }

    // read all food
    printf("nFood = %s\n", nFood);
    for (int i=0; i<nFood; i++) {
        if (!std::getline(f, line)) goto handleErr;
        Food food;
        sscanf(line.c_str(), "[%lf %lf %lf] [%lf %lf %lf] %lf", food.position[0], food.position[1],
                food.position[2], food.velocity[0], food.velocity[1], food.velocity[2], food.time);
        foods.push_back(food);
    }

    return 0; // success
handleErr:
    printf("Error: while reading file %s\n", fname);
    return -1;

}
