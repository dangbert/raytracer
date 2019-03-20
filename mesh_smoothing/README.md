Dan Engbert (end1@umbc.edu), CMSC 435 Fall 18

# Project4
* Program that reads in a mesh file (.obj) and smooths the mesh using Laplacian smoothing.
* Extra credit implemented: implicit integration.


## How to Run:
````bash
make && ./smoothing inputs/bunny.obj out.obj 1 1 50

# run with implicit integration enabled:
make && ./smoothing -i 10e-10 inputs/bunny.obj out.obj 1 1 50
````


## HELP RECEIVED:
* http://w.multires.caltech.edu/pubs/ImplicitFairing.pdf
* https://www.cs.cmu.edu/~quake-papers/painless-conjugate-gradient.pdf
* https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
---
