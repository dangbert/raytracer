#ifndef _TEST_H_
#define _TEST_H_
/* File: Test.h
 * This file defines a class to be used for testing
 * all the other classes in my Tracer library
 * Dan Engbert - UMBC CMSC435 F18 Project 1
 */
#include "Tracer.h"
using Eigen::Vector3d;

class Test {
    public:
    int testRasterizer(std::string filename);
};
#endif
