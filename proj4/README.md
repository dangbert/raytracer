Dan Engbert (end1@umbc.edu), CMSC 435 Fall 18

# Project4
* Program that reads in a mesh file (.obj) and smooths the mesh using Laplacian smoothing.
* Extra credit implemented: implicit integration.


## How to Run:
````bash
make && ./smoothing inputs/bunny.obj out.obj 1 1 50

# run with implicit integration enabled:
# TODO: add command line arg

# TODO: verify this works on gl
````


## HELP RECEIVED:
* http://w.multires.caltech.edu/pubs/ImplicitFairing.pdf
* https://www.cs.cmu.edu/~quake-papers/painless-conjugate-gradient.pdf
---
