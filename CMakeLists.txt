cmake_minimum_required(VERSION 3.31)

if(CMAKE_CXX_MODULE_STD)
    # This specific value changes as experimental support evolves. See
    # `Help/dev/experimental.rst` in the CMake source corresponding to
    # your CMake build for the exact value to use.
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "4.0.3")
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
    elseif(${CMAKE_VERSION} VERSION_GREATER_EQUAL "4.0.2")
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
    else()
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
    endif()
endif()

project(Static-DI VERSION 1.0 LANGUAGES CXX)

option(DI_COMPRESS_TYPES "Compress recursive type names for Context, Impl, Type" FALSE)
option(DI_BUILD_TESTS "Build tests for Static-DI" FALSE)
option(DI_TESTS_LTO "Enable LTO when building tests" TRUE)
option(DI_TESTS_DEBUG_SAN "Enable sanitisers when building tests in debug" TRUE)

if(DI_BUILD_TESTS)
    enable_testing()
    include(tests/config.cmake)
endif()

add_subdirectory(generator_lib)
add_subdirectory(lib)

if(DI_BUILD_TESTS)
    add_subdirectory(tests)

    # GCC PCH does not improve compile times
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_subdirectory(lib/pch)
        target_precompile_headers(di_tests_abc_lib REUSE_FROM di::pch)
        target_precompile_headers(di_tests_headers REUSE_FROM di::tests::abc::lib)
    endif()
endif()
