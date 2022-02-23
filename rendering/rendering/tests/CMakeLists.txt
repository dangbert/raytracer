###### 3rd party libraries
# google test
#   https://google.github.io/googletest/quickstart-cmake.html
include(FetchContent)
FetchContent_Declare(
  googletest
  # note: update this url every now and then (from "main" bracnch of repo)
  URL https://github.com/google/googletest/archive/2377c8d32b82239a29bc81f452ab2875de810b2f.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
######

# let files in this dir include src files
include_directories(../src)

enable_testing()

# TODO: move Test.cpp into this directory?
add_executable(
  testRunner
  # files with gtests:
  TestTriangle.cpp
  TestPolygon.cpp

  # dependencies:
  ../src/Settings.cpp
  ../src/Surfaces.cpp
  ../src/Tracer.cpp
)

target_link_libraries(
  testRunner
  Eigen3::Eigen
  gtest_main # default runner for google tests
)

include(GoogleTest)
gtest_discover_tests(testRunner)