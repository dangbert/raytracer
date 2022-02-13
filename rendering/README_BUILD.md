# cmake-cxx-project
CMake project template.


## Building
```sh
cmake \
    -S . \
    -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DPROJECT_ENABLE_IPO=TRUE \
    -DPROJECT_ENABLE_SANITIZE=FALSE \
    -DPROJECT_ENABLE_STRICT_WARNINGS=FALSE

cmake --build build --config Release
```
