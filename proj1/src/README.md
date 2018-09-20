# Project1
* Simple Implementation of a ray tracer that reads [nff files](https://www.fileformat.info/format/nff/egff.htm)
and renders an image of the 3D scene.
* Only supports rendering polygons at the moment (skips any other object types)
* No shading at the moment

## Help received:
* https://www.fileformat.info/format/nff/egff.htm
* https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html

## STATUS:
* TODO: make sure this compiles on gl (update include to point to Eigen on gl?)
* use hither
* The fill for a surface will be the most recently specified.  That is each fill specification updates the current fill.  That is why I store a vector of <Surface,Fill> pairs.

## NOTE:
### Debugging
I recommend setting up a very simple scene, with a single axis-aligned triangle
at the origin and the camera at (0,0,1) looking at the origin. Then the
calculations become easier to verify by hand. 

### Compile Optimization:
* use the ````-O2```` or ````-O3```` flag
* ````-flto```` turns on link time optimization (harder to debug with this flag on)
* ````-march=native```` saves some more time
tell the compiler to use all instructions available on your cpu

(program may not run on an older cpu if it wasn't compiled there)


### Converting PPM to PNG
````bash
# install ImageMagick, then:
convert image.ppm image.png    # convert to png if desired
display <image_file>           # view the image
````
