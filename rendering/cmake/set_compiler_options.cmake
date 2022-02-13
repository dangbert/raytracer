include_guard(GLOBAL)


include(CheckIPOSupported)
include(GenerateExportHeader)
include(InstallRequiredSystemLibraries)


function(_sco_add_strict_warnings target_name)
    set(flag -Werror)

    set(CMAKE_REQUIRED_QUIET TRUE)
    check_cxx_compiler_flag("${flag}" "CXX_HAS_${flag}")

    if(NOT CXX_HAS_${flag})
        # can fail because CMake triggers a warning...
        # if we have -Weffc++, assume -Werror
        unset("CXX_HAS_${flag}")
        unset("CXX_HAS_${flag}" CACHE)
        check_cxx_compiler_flag(-Weffc++ "CXX_HAS_${flag}")
    endif()

    string(TOUPPER "${PROJECT_NAME}" upper_project_name)
    option(
        "${upper_project_name}_ENABLE_STRICT_WARNINGS"
        "Regard warnings as errors"
        TRUE)

    if(${upper_project_name}_ENABLE_STRICT_WARNINGS AND CXX_HAS_${flag})
        target_compile_options("${target_name}" PRIVATE "${flag}")
    endif()
endfunction()


function(_sco_set_ipo target_name)
    check_ipo_supported(RESULT ipo_supported LANGUAGES CXX)
    string(TOUPPER "${PROJECT_NAME}" upper_project_name)
    option("${upper_project_name}_ENABLE_IPO" "Enable IPO" TRUE)

    if(${upper_project_name}_ENABLE_IPO AND ipo_supported)
        set_property(
            TARGET "${target_name}"
            PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endfunction()


function(_sco_add_compile_options target_name)
    foreach(flag IN LISTS ARGN)
        string(REPLACE " " "_" test_name "${flag}")
        check_cxx_compiler_flag("${flag}" "CXX_HAS_${test_name}")

        if(CXX_HAS_${test_name})
            target_compile_options("${target_name}" PRIVATE "${flag}")
        endif()
    endforeach()
endfunction()


function(_sco_check_linker_flag flag out_var)
    set(CMAKE_REQUIRED_LINK_OPTIONS "${flag}")
    check_cxx_compiler_flag("" "${out_var}")
endfunction()


function(_sco_add_link_options target_name scope)
    foreach(flag IN LISTS ARGN)
        string(REPLACE " " "_" test_name "${flag}")
        _sco_check_linker_flag("${flag}" "CXX_LINKER_HAS_${test_name}")

        if(CXX_LINKER_HAS_${test_name})
            target_link_options("${target_name}" "${scope}" "${flag}")
        endif()
    endforeach()
endfunction()


function(_sco_check_compiler_and_linker_flag flag out_var)
    set(CMAKE_REQUIRED_LINK_OPTIONS "${flag}")
    check_cxx_compiler_flag("${flag}" "${out_var}")
endfunction()


function(
        _sco_add_compile_and_link_options
        target_name
        scope_compile
        scope_link)
    foreach(flag IN LISTS ARGN)
        string(REPLACE " " "_" test_name "${flag}")
        _sco_check_compiler_and_linker_flag(
            "${flag}"
            "CXX_AND_CXX_LINKER_HAVE_${test_name}")

        if(CXX_AND_CXX_LINKER_HAVE_${test_name})
            target_compile_options(
                "${target_name}"
                "${scope_compile}"
                "${flag}")
            target_link_options("${target_name}" "${scope_link}" "${flag}")
        endif()
    endforeach()
endfunction()


function(_sco_add_sanitize target_name)
    string(TOUPPER "${PROJECT_NAME}" upper_project_name)
    option("${upper_project_name}_ENABLE_SANITIZE" "Enable sanitize" FALSE)

    if(${upper_project_name}_ENABLE_SANITIZE)
        target_compile_definitions("${target_name}" PRIVATE _FORTIFY_SOURCE=0)

        _sco_add_compile_options("${target_name}" -fno-sanitize-recover=all)
        _sco_add_compile_and_link_options("${target_name}" PRIVATE PUBLIC
            -fsanitize=address
            -fsanitize=float-cast-overflow
            -fsanitize=float-divide-by-zero
            -fsanitize=undefined)

        get_target_property(target_type "${target_name}" TYPE)

        if (target_type STREQUAL "SHARED_LIBRARY")
            _sco_add_link_options("${target_name}" PUBLIC -shared-libsan)
        endif()
    else()
        target_compile_definitions("${target_name}" PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:_FORTIFY_SOURCE=2>)
    endif()
endfunction()


function(_sco_add_libcpp_debug target_name)
    set(definition _LIBCPP_DEBUG=1)

    set(CMAKE_REQUIRED_DEFINITIONS "-D${definition}")
    check_cxx_source_compiles("
        #include <string>
        #include <vector>
        int main(int argc, char **) {
            std::vector<std::string> v(argc);
            return v.size() != 1;
        }"
        CXX_LIBRARY_HAS_${definition})

    if(CXX_LIBRARY_HAS_${definition})
        target_compile_definitions("${target_name}" PUBLIC
            $<$<CONFIG:Debug>:${definition}>)
    endif()
endfunction()


function(_sco_set_link_what_you_use target_name)
    _sco_check_linker_flag(
        -Wl,--no-as-needed
        CXX_LINKER_HAS_LINK_WHAT_YOU_USE)

    if(CXX_LINKER_HAS_LINK_WHAT_YOU_USE)
        set_property(TARGET "${target_name}" PROPERTY LINK_WHAT_YOU_USE TRUE)
    endif()
endfunction()


function(set_compiler_options target_name)
    set_target_properties("${target_name}" PROPERTIES
        BUILD_RPATH_USE_ORIGIN TRUE
        CXX_EXTENSIONS FALSE
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED TRUE
        CXX_VISIBILITY_PRESET hidden
        POSITION_INDEPENDENT_CODE TRUE
        PREFIX "")

    _sco_add_libcpp_debug("${target_name}")
    _sco_add_sanitize("${target_name}")
    _sco_add_strict_warnings("${target_name}")
    _sco_set_ipo("${target_name}")
    _sco_set_link_what_you_use("${target_name}")

    target_compile_definitions("${target_name}" PRIVATE
        _GLIBCXX_ASSERTIONS
        $<$<CONFIG:Debug>:_GLIBCXX_DEBUG_PEDANTIC>)

    target_compile_definitions("${target_name}" PUBLIC
        $<$<CONFIG:Debug>:_GLIBCXX_DEBUG>)

    get_target_property(target_type "${target_name}" TYPE)

    if(NOT target_type STREQUAL "EXECUTABLE")
        generate_export_header("${target_name}")
        target_include_directories("${target_name}" PUBLIC
            "${CMAKE_CURRENT_BINARY_DIR}")
    endif()

    _sco_add_compile_options("${target_name}" -Wfatal-errors)

    _sco_add_compile_options("${target_name}"
        -Wall
        -Weffc++
        -Weverything
        -Wextra
        -Wpedantic
        /permissive-)

    _sco_add_compile_options("${target_name}"
        -Waligned-new=all
        -Walloc-zero
        -Walloca
        -Warray-bounds=2
        -Wattribute-alias=2
        -Wcast-align=strict
        -Wcast-qual
        -Wcatch-value=3
        -Wcomma-subscript
        -Wconditionally-supported
        -Wconversion
        -Wctad-maybe-unsupported
        -Wctor-dtor-privacy
        -Wdate-time
        -Wdeprecated-copy-dtor
        -Wdeprecated-enum-enum-conversion
        -Wdeprecated-enum-float-conversion
        -Wdisabled-optimization
        -Wdouble-promotion
        -Wduplicated-branches
        -Wduplicated-cond
        -Wextra-semi
        -Wfloat-equal
        -Wformat-overflow=2
        -Wformat-signedness
        -Wformat-truncation=2
        -Wformat=2
        -Wframe-larger-than=1024
        -Wimplicit-fallthrough=5
        -Winline
        -Winvalid-pch
        -Wlarger-than=1024
        -Wlogical-op
        -Wmismatched-tags
        -Wmissing-braces
        -Wmissing-declarations
        -Wmissing-include-dirs
        -Wmultichar
        -Wmultiple-inheritance
        -Wnoexcept
        -Wnull-dereference
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpacked
        -Wpadded
        -Wplacement-new=2
        -Wredundant-decls
        -Wredundant-tags
        -Wregister
        -Wshadow
        -Wshift-overflow=2
        -Wsign-conversion
        -Wsign-promo
        -Wstack-protector
        -Wstack-usage=1024
        -Wstrict-null-sentinel
        -Wstrict-overflow=5
        -Wstringop-overflow=4
        -Wsuggest-attribute=format
        -Wsuggest-attribute=noreturn
        -Wsuggest-final-methods
        -Wsuggest-final-types
        -Wsuggest-override
        -Wswitch-default
        -Wswitch-enum
        -Wtrampolines
        -Wundef
        -Wuninitialized
        -Wunused-macros
        -Wuseless-cast
        -Wvector-operation-performance
        -Wvirtual-inheritance
        -Wvolatile
        -Wzero-as-null-pointer-constant)

    _sco_add_compile_options("${target_name}" -Wno-c++98-compat)

    _sco_add_compile_options("${target_name}"
        --param=ssp-buffer-size=4
        -fasynchronous-unwind-tables
        -fcf-protection=full
        -fdevirtualize-at-ltrans
        -fno-plt
        -fno-semantic-interposition
        -fstack-clash-protection
        -fstack-protector-all
        -ftrapv
        -mshstk
        -pipe)

    _sco_add_link_options("${target_name}" PRIVATE
        -Wl,--sort-common
        -Wl,-z,defs
        -Wl,-z,noexecstack
        -Wl,-z,now
        -Wl,-z,relro)
endfunction()
