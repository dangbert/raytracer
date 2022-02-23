#!/bin/bash
set -euC


cd "$(dirname "${0}")"


readonly config=Release
readonly project_name=rendering
readonly source_path="${PWD}"
readonly build_path="${PWD}/build/${config}"
readonly install_path="${build_path}/install"


if ! test -e "${build_path}"; then
    readonly upper_project_name="$(echo "${project_name}" |
        tr '[:lower:]' '[:upper:]')"

    cmake \
        -G Ninja \
        -S "${source_path}" \
        -B "${build_path}" \
        "-D${upper_project_name}_ENABLE_IPO=FALSE" \
        "-D${upper_project_name}_ENABLE_SANITIZE=TRUE" \
        "-D${upper_project_name}_ENABLE_STRICT_WARNINGS=TRUE" \
        "-DCMAKE_BUILD_TYPE=${config}" \
        "-DCMAKE_CXX_COMPILER=g++" \
        "-DCMAKE_INSTALL_PREFIX=${install_path}" \
        -DBUILD_SHARED_LIBS=TRUE \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
        -DEigen3_DIR=../libraries/eigen-3.4.0_install
fi


cmake --build "${build_path}" --config "${config}"
# building with single thread:
#cmake --build "${build_path}" --config "${config}" --parallel=1

# TODO: find more elegant way to make/clean these softlinks?
! [ -f raytracer ] && ln -s build/Release/rendering/raytracer .
! [ -f rasterizer ] && ln -s build/Release/rendering/rasterizer .

cd "${build_path}"
ctest \
    #-V \ # verbose
    --build-config "${config}" \
    --output-on-failure \
    --schedule-random \
    --stop-on-failure
cd -
