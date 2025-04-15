add_subdirectory(abc)

find_package(doctest REQUIRED)

add_library(di_test test_main.cpp)
add_library(di::test ALIAS di_test)
target_compile_definitions(di_test PUBLIC DOCTEST_CONFIG_SUPER_FAST_ASSERTS=1)

target_link_libraries(di_test PUBLIC doctest::doctest)

add_executable(di_test_headers
    test_abc_headers.cpp
    test_embedded_headers.cpp
)
add_executable(di_test_module
    test_abc_module.cpp
    test_charlie.cpp
    test_embedded_modules.cpp
    test_mock.cpp
    test_union.cpp
    repeater/main.cpp
    thread/main.cpp
)

set_property(TARGET di_test_headers di_test_module
    PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

target_generate_di_headers(di_test_headers
    EMBED test_embedded_headers.cpp di/tests/test_embedded_headers.hxx
)

target_generate_di_modules(di_test_module
    GLOB
        repeater
        thread
    EMBED
        test_embedded_modules.cpp
        test_mock.cpp
)

target_sources(di_test_module
    PUBLIC FILE_SET CXX_MODULES FILES
        repeater/test_node.ixx
        thread/poster.ixx
        thread/test_node.ixx
)

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
