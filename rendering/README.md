# Raytracer and Rasterizer

## Dependencies:

This code depends on the [Eigen library](http://eigen.tuxfamily.org/index.php?title=Main_Page#Download) (see all versions [here](https://gitlab.com/libeigen/eigen/-/releases)).
* [Version 3.4.0](https://gitlab.com/libeigen/eigen/-/releases/3.4.0) seems to work well (without any specific warnings during compile).  Download it and extract it to the folder `./libraries/eigen-3.4.4` in the root of this repo.

"Build" eigen so we can link it with our cmake system:
````bash
cmake -S eigen-3.4.0 -B eigen-build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PWD/eigen-3.4.0_install"
cmake --build eigen-build && cmake --install eigen-build && rm -rf eigen-build
````