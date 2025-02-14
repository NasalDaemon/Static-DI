add_library(di_abc_lib)
add_library(di::abc::lib ALIAS di_abc_lib)

target_link_libraries(di_abc_lib PUBLIC di::lib)

target_include_directories(di_abc_lib PUBLIC include)
target_generate_di_headers(di_abc_lib INCLUDE_DIR include
    # GLOB
    #     abc
    # FILES
    #     abc/graph.hxx.dig
)

target_generate_di_src(di_abc_lib
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
