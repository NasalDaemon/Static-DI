cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)

if(CMAKE_CXX_MODULE_STD)
    # This specific value changes as experimental support evolves. See
    # `Help/dev/experimental.rst` in the CMake source corresponding to
    # your CMake build for the exact value to use.
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
    set(CMAKE_CXX_FLAGS "-stdlib=libc++")
endif()

project(Static-DI VERSION 1.0 LANGUAGES CXX)

if(DI_BUILD_TESTS)
    include(tests.cmake)
else()
    add_subdirectory(di)
endif()
