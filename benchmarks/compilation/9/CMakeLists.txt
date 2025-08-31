add_executable(di_bench_compile9 main.cpp)
target_link_libraries(di_bench_compile9 di::module)
target_generate_di_modules(di_bench_compile9)

if(DI_BUILD_TESTS)
    add_test(di_bench_compile9_test di_bench_compile9)
endif()

if(DI_BUILD_LTO)
    set_property(TARGET di_bench_compile9 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

file(GLOB NODE_IXX_FILES "nodes/*.ixx")
target_sources(di_bench_compile9
    PUBLIC FILE_SET CXX_MODULES FILES
        ${NODE_IXX_FILES}
)

target_generate_di_src(di_bench_compile9
    GRAPH_MODULE di.bench.compile9.graph
    GRAPH_TYPE   di::Graph<di::bench::compile9::Graph>
    NODES
        node1    di.bench.compile9.node1
        node2    di.bench.compile9.node2
        node3    di.bench.compile9.node3
        node4    di.bench.compile9.node4
        node5    di.bench.compile9.node5
        node6    di.bench.compile9.node6
        node7    di.bench.compile9.node7
        node8    di.bench.compile9.node8
        node9    di.bench.compile9.node9
)
