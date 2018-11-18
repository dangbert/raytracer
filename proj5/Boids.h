#ifndef _BOID_H_
#define _BOID_H_
/* File: Boid.h
 * This file defines the Boid class which represents a single member of a flock
 *  Dan Engbert - UMBC CMSC435 F18
 */
#include <Eigen/Dense>
#include <vector>
using Eigen::Vector3d;

// forward declaration
class Flock;

/**
 * represents a piece of food in the simulation
 */
struct Food {
    Vector3d position;
    Vector3d velocity;
    double time; // time the food should appear
};

/**
 *  represents a member of a flock
 */
class Boid {
    friend class Flock;
    public:
        Boid(Vector3d position=Vector3d(0,0,0), Vector3d heading=Vector3d(0,0,0), Vector3d velocity=Vector3d(0,0,0)) 
            : position(position), heading(heading), velocity(velocity) {};

    private:
        Vector3d position;
        Vector3d heading;
        Vector3d velocity;
};

/**
 * runs a simulation
 * and stores all info about the simulation
 */
class Flock {
    public:
        Flock(std::string fname);
        void readFile(std::string fname);

    private:
        inline void getLineHelper(std::ifstream &f, std::string &line);
        std::vector<Boid> boids; // all boids in simulation
        std::vector<Food> foods; // all food in simulation
        double bSize;       // default size of a boid)
        double nRadius;     // distance a boid can see
        double nNeigbors;   // max number of neighboring boids to process
        double mass;        // mass of a boid
        double wCollision;  // weight of collision force
        double wCentering;  // weight of centering force
        double wVelocity;   // weight of velocity force
        double wHunger;     // weight of foce drawing boids to nearest food
        double damping;     // "mass proportional damping" constant (multiply w/ velocity each timestep)
        double dt;          // timestep size
        double duration;    // duration of animation (in seconds) (# frames = 30 * duration)
};

#endif
