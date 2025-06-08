add_subdirectory(abc)

find_package(doctest REQUIRED)

add_library(di_tests test_main.cpp)
add_library(di::tests ALIAS di_tests)
target_compile_definitions(di_tests PUBLIC DOCTEST_CONFIG_SUPER_FAST_ASSERTS=1)

target_link_libraries(di_tests PUBLIC doctest::doctest)

add_executable(di_tests_headers
    test_abc_headers.cpp
    test_embedded_headers.cpp
)
add_executable(di_tests_module
    test_abc_module.cpp
    test_embedded_modules.cpp
    test_node_count.cpp
    test_union.cpp
    test_virtual.cpp
    test_repeater/main.cpp
    test_thread/main.cpp
)
add_executable(di_tests_mock
    test_combine.cpp
    test_lazy.cpp
    test_mock.cpp
)

if(DI_TESTS_LTO)
    set_property(TARGET di_tests_headers di_tests_module
        PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

    # mocks don't work well with GCC-14+LTO
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "15.1.0")
        set_property(TARGET di_tests_mock
            PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

target_generate_di_headers(di_tests_headers
    EMBED test_embedded_headers.cpp di/tests/test_embedded_headers.hxx
)

target_generate_di_modules(di_tests_module
    GLOB
        test_*
    EMBED
        test_embedded_modules.cpp
        test_node_count.cpp
        test_virtual.cpp
)

target_generate_di_modules(di_tests_mock
    EMBED
        test_mock.cpp
)

target_sources(di_tests_module
    PUBLIC FILE_SET CXX_MODULES FILES
        test_repeater/test_node.ixx
        test_thread/poster.ixx
        test_thread/test_node.ixx
)

target_generate_di_src(di_tests_module
    GRAPH_MODULE abc.graph
    GRAPH_TYPE   di::Graph<abc::AliceBob>
    NODES
        alice    abc.alice
)

target_link_libraries(di_tests_headers PUBLIC
    di::tests::abc::lib
    di::tests
)
target_link_libraries(di_tests_module PUBLIC
    di::tests::abc::module
    di::tests
)
target_link_libraries(di_tests_mock PUBLIC
    di::module
    di::tests
)

add_test(di_tests_headers di_tests_headers)
add_test(di_tests_module di_tests_module)
add_test(di_tests_mock di_tests_mock)
