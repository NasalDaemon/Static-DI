add_library(di_tests_abc_lib)
add_library(di::tests::abc::lib ALIAS di_tests_abc_lib)

add_library(di_tests_abc_module_lib)
add_library(di::tests::abc::module ALIAS di_tests_abc_module_lib)

if(DI_BUILD_LTO)
    set_property(TARGET di_tests_abc_lib di_tests_abc_module_lib
        PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_link_libraries(di_tests_abc_lib PUBLIC di::di)
target_link_libraries(di_tests_abc_module_lib PUBLIC di::module)

target_include_directories(di_tests_abc_lib PUBLIC headers/include)
target_generate_di_headers(di_tests_abc_lib INCLUDE_DIR headers/include
    # GLOB
    #     abc
    # FILES
    #     abc/graph.hxx.dig
)

target_generate_di_src(di_tests_abc_lib
    # PRECOMPILE_HEADERS
    # ID 1
    # UNITY
    # COMMON_HEADERS
    #     abc/alice.hpp
    GRAPH_HEADER abc/graph.hxx
    GRAPH_TYPE   di::Graph<abc::AliceBob>
    NODES
        # alice         , bob         , charlie.charlie
        # abc/alice.tpp , abc/bob.tpp , abc/charlie.tpp
        alice           abc/alice.tpp
        bob             abc/bob.tpp
        charlie.charlie abc/charlie.tpp
)

target_generate_di_modules(di_tests_abc_module_lib)

target_sources(di_tests_abc_lib
    PRIVATE
        headers/src/alice.cpp
        headers/src/ellie.cpp
)

target_sources(di_tests_abc_module_lib
    PUBLIC FILE_SET CXX_MODULES FILES
        modules/abc/alice.ixx
        modules/abc/alice.impl.ixx
        modules/abc/bob.ixx
        modules/abc/charlie.ixx
        modules/abc/ellie.ixx
        modules/abc/graph.ixx
    PRIVATE
        modules/abc/alice.cpp
        modules/abc/ellie.cpp
)
