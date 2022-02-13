set -euC


cd "$(dirname "${0}")"


readonly config=Release
readonly project_name=project
readonly source_path="${PWD}/../${project_name}"
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
        -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE
fi


cmake --build "${build_path}" --config "${config}"


cd "${build_path}"
ctest \
    --build-config "${config}" \
    --output-on-failure \
    --schedule-random \
    --stop-on-failure
cd -
