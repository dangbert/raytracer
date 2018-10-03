Dan Engbert (end1@umbc.edu), CMSC 435 Fall 18

# Project2
* Simple Implementation of a ray tracer that reads [nff files](https://www.fileformat.info/format/nff/egff.htm)
and renders an image of the 3D scene.
  * Supports polygons and spheres
  * Blinn-Phong diffuse and specular shading
  * shadows

## How to Run:
````bash
make && ./proj2 <input.ppm>
````
(file will be written to ./out.ppm)

## HELP RECEIVED:
* https://www.fileformat.info/format/nff/egff.htm
* https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html
* Textbook: Fundamentals of Computer Graphics 3rd Edition
---

## PERSONAL NOTES:
#### Code Organization
* Surfaces.h // geometric objects
* Settings.h // nff settings, fill types
* Tracer.h   // RayTracer, Ray

#### Compile Optimization:
* use the ````-O2```` or ````-O3```` flag
* ````-flto```` turns on link time optimization (harder to debug with this flag on)
* ````-march=native```` saves some more time
tell the compiler to use all instructions available on your cpu (may not run on an older cpu if it wasn't compiled there).

#### Converting PPM to PNG
````bash
# install ImageMagick, then:
convert image.ppm image.png    # convert to png if desired
display <image_file>           # view the image
````
