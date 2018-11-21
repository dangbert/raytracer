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
    Vector3d pos;
    Vector3d vel;
    double time; // time the food should appear
};

/**
 *  represents a member of a flock
 */
class Boid {
    friend class Flock;
    public:
        Boid(Vector3d pos=Vector3d(0,0,0), Vector3d vel=Vector3d(0,0,0)) 
            : pos(pos), vel(vel) {};
        inline bool inBox();

    private:
        Vector3d pos;     // current position
        Vector3d vel;     // velocity (speed and direction)
        std::vector<int> neighs; // neighbors of this bird (indices in Flock::boids)
        Vector3d frc;        // force for current timestep
        bool wasInBox = true;
};

/**
 * runs a simulation
 * and stores all info about the simulation
 */
class Flock {
    public:
        Flock(std::string fname);
        void readFile(std::string fname);
        void simulate();

    private:
        void computeForces();
        inline void getLineHelper(std::ifstream &f, std::string &line);
        void writeOutput();

        std::vector<Boid> boids; // all boids in simulation
        std::vector<Food> foods; // all food in simulation
        double bSize;       // default size of a boid (spherical radius)
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
