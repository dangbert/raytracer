# TODO: see Jon's libproject/include (for building a library)
#   (and add export headers to my classes)

#add_library(libproject src/project.cpp)
#target_include_directories(libproject PUBLIC
#    "${CMAKE_CURRENT_SOURCE_DIR}/include")

# 3rd party library
find_package (Eigen3 3.4.0 REQUIRED NO_MODULE)

add_executable(
    raytracer
    src/main-raytracer.cpp
    src/Settings.cpp
    src/Surfaces.cpp
    src/Tracer.cpp
    src/Rasterizer.cpp # needed for Test.cpp to compile
    src/Test.cpp
)
target_link_libraries (raytracer Eigen3::Eigen)
set_compiler_options(raytracer)

add_executable(
    rasterizer
    src/main-rasterizer.cpp
    src/Rasterizer.cpp
    src/Settings.cpp
    src/Surfaces.cpp
    src/Tracer.cpp # needed for Test.cpp to compile
    src/Test.cpp
)
target_link_libraries (rasterizer Eigen3::Eigen)
set_compiler_options(rasterizer)

add_subdirectory(tests)
