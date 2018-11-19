#include "Boids.h"
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>

using Eigen::Vector3d;
using std::cout;
using std::cerr;
using std::endl;

////////////////////////////
/////// Flock class: ///////
////////////////////////////
Flock::Flock(std::string fname) {
    readFile(fname);

    cerr << "bSize " << bSize << endl;
    cerr << "nRadius " << nRadius << endl;
    cerr << "nNeigbors " << nNeigbors << endl;
    cerr << "mass " << mass << endl;
    cerr << "wCollision " << wCollision << endl;
    cerr << "wCentering " << wCentering << endl;
    cerr << "wVelocity " << wVelocity << endl;
    cerr << "wHunger " << wHunger << endl;
    cerr << "damping " << damping << endl;
    cerr << "dt " << dt << endl;
    cerr << "duration " << duration << endl;
}

/**
 * run the simulation and print out the locations of the objects
 * at each time frame
 *
 * note: a kdtree could be used to speed things up
 * TODO: have a maximum acceleration (defined as a fraction of the max speed)
 */
void Flock::simulate() {
    cerr << "\nstarting simulation (duration=" << duration << ", dt=" << dt << "):" << endl;

    int nFrames = 0;
    for (double t=0; t<duration; t+=dt) {
        nFrames++;
    }
    // first line of output is the number of frames
    cout << nFrames << endl;

    for (double t=0; t<duration; t+=dt) {
        computeForces();
        for (unsigned int i=0; i<boids.size(); i++) {
            // acceleration -> change in velocity
            boids[i].vel += damping * (boids[i].frc * dt);
            // velocity -> change in position:
            boids[i].pos += boids[i].vel * dt;
        }
        writeOutput();
    }
}

void Flock::computeForces() {
    // TODO NOTE: within the raidus nRadius: (p. 6 in pdf)
    //   The magnitude of the sensitivity is defined as an inverse exponential of distance
    //   (neighborhood defined by radius and exponent)
    //   also see: "Details about my setup" on assignment page
    //   note: in picture of board from class, k_coll and k_centering, k_vel and the weights of each force
    for (unsigned int i=0; i<boids.size(); i++) {
        double neighbors = 0;
        boids[i].frc = Vector3d(0,0,0);
        Vector3d center = Vector3d(0,0,0); // center of local flock
        // search for neighbors of this boid
        for (unsigned int j=0; j<boids.size(); j++) {
            if (j == i)
                continue;
            // check if this is a neighbor
            double dist = (boids[j].pos - boids[i].pos).norm();
            if (dist < nRadius) {
                // j is a neighbor of i:
                neighbors++;
                center += boids[j].pos;

                boids[i].frc += wVelocity*(boids[j].vel - boids[i].vel);
                // collision avoidance: move away from this neighbor (inversely proportional to dist^3)
                boids[i].frc += wCollision*((boids[j].pos - boids[i].pos) / pow(dist, 3));
            }
        }

        // now calculate the combined force for this boid (weighted average):
        // calculate the center of this boid's neighbors
        center = (neighbors != 0) ? (center/neighbors) : boids[i].pos;
        // by using the (un-normalized) vector between the center and current position, this force is weighted by distance
        boids[i].frc += wCentering*(center - boids[i].pos);
    }
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
        sscanf(line.c_str(), "[%lf,%lf,%lf] [%lf,%lf,%lf]", &boi.pos[0], &boi.pos[1],
                &boi.pos[2], &boi.vel[0], &boi.vel[1], &boi.vel[2]);
        boids.push_back(boi);
    }

    // read line with nFood
    getLineHelper(f, line);
    sscanf(line.c_str(), "%d", &nFood);
    // read all food
    for (int i=0; i<nFood; i++) {
        getLineHelper(f, line);
        Food food;
        sscanf(line.c_str(), "[%lf,%lf,%lf] [%lf,%lf,%lf] %lf", &food.pos[0], &food.pos[1],
                &food.pos[2], &food.vel[0], &food.vel[1], &food.vel[2], &food.time);
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

/**
 * write the output to stdout
 */
void Flock::writeOutput() {
    cout << boids.size() << std::endl;
    for (unsigned int i=0; i<boids.size(); i++) {
        cout << "[" << boids[i].pos[0] << "," << boids[i].pos[1] << "," << boids[i].pos[2] << "] ";
        cout << "[" << boids[i].vel[0] << "," << boids[i].vel[1] << "," << boids[i].vel[2] << "]" << std::endl;
    }
    cout << foods.size() << std::endl;
    for (unsigned int i=0; i<foods.size(); i++) {
        cout << "[" << foods[i].pos[0] << "," << foods[i].pos[1] << "," << foods[i].pos[2] << "]" << std::endl;
    }
}
