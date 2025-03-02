cmake_minimum_required(VERSION 3.28)

if(CMAKE_CXX_MODULE_STD)
    # This specific value changes as experimental support evolves. See
    # `Help/dev/experimental.rst` in the CMake source corresponding to
    # your CMake build for the exact value to use.
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
endif()

project(Static-DI VERSION 1.0 LANGUAGES CXX)

if(DI_BUILD_TESTS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        string(APPEND CMAKE_CXX_FLAGS " -Werror -Wall -Wextra -Wpedantic -fcolor-diagnostics")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-ld=mold -lc++abi")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        string(APPEND CMAKE_CXX_FLAGS " "
            "-Werror -Wall -Wextra -Wpedantic "
            "-fdiagnostics-all-candidates -fconcepts-diagnostics-depth=5 "
            "-fdiagnostics-color=always "
            # Unfortunately, GCC LTO partitioning and modules do not mix well (symbols often missing at link time)
            "-fno-fat-lto-objects -flto-partition=none "
        )
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-ld=mold")
    endif()
endif()

add_subdirectory(generator_lib)
add_subdirectory(lib)

if(DI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    # GCC PCH does not improve compile times
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_subdirectory(lib/pch)
        target_precompile_headers(di_abc_lib REUSE_FROM di::pch)
        target_precompile_headers(di_test_headers REUSE_FROM di::abc::lib)
    endif()
endif()
