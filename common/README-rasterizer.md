Dan Engbert (end1@umbc.edu), CMSC 435 Fall 18

# Project3
* Simple Implementation of a rasterizer that reads [nff files](https://www.fileformat.info/format/nff/egff.htm)
and renders an image of the 3D scene.
  * Supports polygons and polygon patches
* NOTE: The code for this project is primarily located in ../common

## How to Run:
````bash
make && ./proj3 <input.ppm>
````
(file will be written to ./out.ppm)

## HELP RECEIVED:
* https://www.fileformat.info/format/nff/egff.htm
* Textbook: Fundamentals of Computer Graphics 3rd Edition
* https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
---

## PERSONAL NOTES:
#### Converting PPM to PNG
````bash
# install ImageMagick, then:
convert image.ppm image.png    # convert to png if desired
display <image_file>           # view the image
````
