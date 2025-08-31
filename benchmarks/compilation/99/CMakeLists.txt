add_executable(di_bench_compile99 main.cpp)
target_link_libraries(di_bench_compile99 di::module)
target_generate_di_modules(di_bench_compile99)

if(DI_BUILD_TESTS)
    add_test(di_bench_compile99_test di_bench_compile99)
endif()

if(DI_BUILD_LTO)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # flto=thin doesn't collapse benchmark99 into return 0
        target_compile_options(di_bench_compile99 PRIVATE "-flto=full")
        target_link_options(di_bench_compile99 PRIVATE "-flto=full")
    else()
        set_property(TARGET di_bench_compile99 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

file(GLOB NODE_IXX_FILES "nodes/*.ixx")
target_sources(di_bench_compile99
    PUBLIC FILE_SET CXX_MODULES FILES
        ${NODE_IXX_FILES}
)

target_generate_di_src(di_bench_compile99
    GRAPH_MODULE di.bench.compile99.graph
    GRAPH_TYPE   di::Graph<di::bench::compile99::Graph>
    NODES
        node1    di.bench.compile99.node1
        node2    di.bench.compile99.node2
        node3    di.bench.compile99.node3
        node4    di.bench.compile99.node4
        node5    di.bench.compile99.node5
        node6    di.bench.compile99.node6
        node7    di.bench.compile99.node7
        node8    di.bench.compile99.node8
        node9    di.bench.compile99.node9
        node10   di.bench.compile99.node10
        node11   di.bench.compile99.node11
        node12   di.bench.compile99.node12
        node13   di.bench.compile99.node13
        node14   di.bench.compile99.node14
        node15   di.bench.compile99.node15
        node16   di.bench.compile99.node16
        node17   di.bench.compile99.node17
        node18   di.bench.compile99.node18
        node19   di.bench.compile99.node19
        node20   di.bench.compile99.node20
        node21   di.bench.compile99.node21
        node22   di.bench.compile99.node22
        node23   di.bench.compile99.node23
        node24   di.bench.compile99.node24
        node25   di.bench.compile99.node25
        node26   di.bench.compile99.node26
        node27   di.bench.compile99.node27
        node28   di.bench.compile99.node28
        node29   di.bench.compile99.node29
        node30   di.bench.compile99.node30
        node31   di.bench.compile99.node31
        node32   di.bench.compile99.node32
        node33   di.bench.compile99.node33
        node34   di.bench.compile99.node34
        node35   di.bench.compile99.node35
        node36   di.bench.compile99.node36
        node37   di.bench.compile99.node37
        node38   di.bench.compile99.node38
        node39   di.bench.compile99.node39
        node40   di.bench.compile99.node40
        node41   di.bench.compile99.node41
        node42   di.bench.compile99.node42
        node43   di.bench.compile99.node43
        node44   di.bench.compile99.node44
        node45   di.bench.compile99.node45
        node46   di.bench.compile99.node46
        node47   di.bench.compile99.node47
        node48   di.bench.compile99.node48
        node49   di.bench.compile99.node49
        node50   di.bench.compile99.node50
        node51   di.bench.compile99.node51
        node52   di.bench.compile99.node52
        node53   di.bench.compile99.node53
        node54   di.bench.compile99.node54
        node55   di.bench.compile99.node55
        node56   di.bench.compile99.node56
        node57   di.bench.compile99.node57
        node58   di.bench.compile99.node58
        node59   di.bench.compile99.node59
        node60   di.bench.compile99.node60
        node61   di.bench.compile99.node61
        node62   di.bench.compile99.node62
        node63   di.bench.compile99.node63
        node64   di.bench.compile99.node64
        node65   di.bench.compile99.node65
        node66   di.bench.compile99.node66
        node67   di.bench.compile99.node67
        node68   di.bench.compile99.node68
        node69   di.bench.compile99.node69
        node70   di.bench.compile99.node70
        node71   di.bench.compile99.node71
        node72   di.bench.compile99.node72
        node73   di.bench.compile99.node73
        node74   di.bench.compile99.node74
        node75   di.bench.compile99.node75
        node76   di.bench.compile99.node76
        node77   di.bench.compile99.node77
        node78   di.bench.compile99.node78
        node79   di.bench.compile99.node79
        node80   di.bench.compile99.node80
        node81   di.bench.compile99.node81
        node82   di.bench.compile99.node82
        node83   di.bench.compile99.node83
        node84   di.bench.compile99.node84
        node85   di.bench.compile99.node85
        node86   di.bench.compile99.node86
        node87   di.bench.compile99.node87
        node88   di.bench.compile99.node88
        node89   di.bench.compile99.node89
        node90   di.bench.compile99.node90
        node91   di.bench.compile99.node91
        node92   di.bench.compile99.node92
        node93   di.bench.compile99.node93
        node94   di.bench.compile99.node94
        node95   di.bench.compile99.node95
        node96   di.bench.compile99.node96
        node97   di.bench.compile99.node97
        node98   di.bench.compile99.node98
        node99   di.bench.compile99.node99
)
