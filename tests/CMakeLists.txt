add_subdirectory(abc)

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

find_package(doctest REQUIRED)

add_library(di_test test_main.cpp)
add_library(di::test ALIAS di_test)
target_compile_definitions(di_test PUBLIC DOCTEST_CONFIG_SUPER_FAST_ASSERTS=1)

target_link_libraries(di_test PUBLIC doctest::doctest)

add_executable(di_test_headers
    test_abc_headers.cpp
)
add_executable(di_test_module
    test_abc_module.cpp
    test_charlie.cpp
    test_union.cpp
    repeater/main.cpp
    thread/main.cpp
)

set_property(TARGET di_test_headers di_test_module
    PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

target_sources(di_test_module
    PUBLIC FILE_SET CXX_MODULES FILES
        repeater/test_node.ixx
        thread/poster.ixx
        thread/test_node.ixx
)
target_generate_di_modules(di_test_module GLOB repeater thread)

target_link_libraries(di_test_headers PUBLIC
    di::test::abc::lib
    di::test
)
target_link_libraries(di_test_module PUBLIC
    di::test::abc::module
    di::test
)

add_test(di_test_headers di_test_headers)
add_test(di_test_module di_test_module)
