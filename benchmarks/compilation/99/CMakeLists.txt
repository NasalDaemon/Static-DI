add_executable(di_bench_compile99 main.cpp)
target_link_libraries(di_bench_compile99 di::module)
target_generate_di_modules(di_bench_compile99)

if(DI_BUILD_TESTS)
    add_test(di_bench_compile99_test di_bench_compile99)
endif()

if(DI_BUILD_LTO)
    set_property(TARGET di_bench_compile99 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_sources(di_bench_compile99
    PUBLIC FILE_SET CXX_MODULES FILES
        nodes/node1.ixx
        nodes/node1.impl.ixx
        nodes/node2.ixx
        nodes/node2.impl.ixx
        nodes/node3.ixx
        nodes/node3.impl.ixx
        nodes/node4.ixx
        nodes/node4.impl.ixx
        nodes/node5.ixx
        nodes/node5.impl.ixx
        nodes/node6.ixx
        nodes/node6.impl.ixx
        nodes/node7.ixx
        nodes/node7.impl.ixx
        nodes/node8.ixx
        nodes/node8.impl.ixx
        nodes/node9.ixx
        nodes/node9.impl.ixx
        nodes/node10.ixx
        nodes/node10.impl.ixx
        nodes/node11.ixx
        nodes/node11.impl.ixx
        nodes/node12.ixx
        nodes/node12.impl.ixx
        nodes/node13.ixx
        nodes/node13.impl.ixx
        nodes/node14.ixx
        nodes/node14.impl.ixx
        nodes/node15.ixx
        nodes/node15.impl.ixx
        nodes/node16.ixx
        nodes/node16.impl.ixx
        nodes/node17.ixx
        nodes/node17.impl.ixx
        nodes/node18.ixx
        nodes/node18.impl.ixx
        nodes/node19.ixx
        nodes/node19.impl.ixx
        nodes/node20.ixx
        nodes/node20.impl.ixx
        nodes/node21.ixx
        nodes/node21.impl.ixx
        nodes/node22.ixx
        nodes/node22.impl.ixx
        nodes/node23.ixx
        nodes/node23.impl.ixx
        nodes/node24.ixx
        nodes/node24.impl.ixx
        nodes/node25.ixx
        nodes/node25.impl.ixx
        nodes/node26.ixx
        nodes/node26.impl.ixx
        nodes/node27.ixx
        nodes/node27.impl.ixx
        nodes/node28.ixx
        nodes/node28.impl.ixx
        nodes/node29.ixx
        nodes/node29.impl.ixx
        nodes/node30.ixx
        nodes/node30.impl.ixx
        nodes/node31.ixx
        nodes/node31.impl.ixx
        nodes/node32.ixx
        nodes/node32.impl.ixx
        nodes/node33.ixx
        nodes/node33.impl.ixx
        nodes/node34.ixx
        nodes/node34.impl.ixx
        nodes/node35.ixx
        nodes/node35.impl.ixx
        nodes/node36.ixx
        nodes/node36.impl.ixx
        nodes/node37.ixx
        nodes/node37.impl.ixx
        nodes/node38.ixx
        nodes/node38.impl.ixx
        nodes/node39.ixx
        nodes/node39.impl.ixx
        nodes/node40.ixx
        nodes/node40.impl.ixx
        nodes/node41.ixx
        nodes/node41.impl.ixx
        nodes/node42.ixx
        nodes/node42.impl.ixx
        nodes/node43.ixx
        nodes/node43.impl.ixx
        nodes/node44.ixx
        nodes/node44.impl.ixx
        nodes/node45.ixx
        nodes/node45.impl.ixx
        nodes/node46.ixx
        nodes/node46.impl.ixx
        nodes/node47.ixx
        nodes/node47.impl.ixx
        nodes/node48.ixx
        nodes/node48.impl.ixx
        nodes/node49.ixx
        nodes/node49.impl.ixx
        nodes/node50.ixx
        nodes/node50.impl.ixx
        nodes/node51.ixx
        nodes/node51.impl.ixx
        nodes/node52.ixx
        nodes/node52.impl.ixx
        nodes/node53.ixx
        nodes/node53.impl.ixx
        nodes/node54.ixx
        nodes/node54.impl.ixx
        nodes/node55.ixx
        nodes/node55.impl.ixx
        nodes/node56.ixx
        nodes/node56.impl.ixx
        nodes/node57.ixx
        nodes/node57.impl.ixx
        nodes/node58.ixx
        nodes/node58.impl.ixx
        nodes/node59.ixx
        nodes/node59.impl.ixx
        nodes/node60.ixx
        nodes/node60.impl.ixx
        nodes/node61.ixx
        nodes/node61.impl.ixx
        nodes/node62.ixx
        nodes/node62.impl.ixx
        nodes/node63.ixx
        nodes/node63.impl.ixx
        nodes/node64.ixx
        nodes/node64.impl.ixx
        nodes/node65.ixx
        nodes/node65.impl.ixx
        nodes/node66.ixx
        nodes/node66.impl.ixx
        nodes/node67.ixx
        nodes/node67.impl.ixx
        nodes/node68.ixx
        nodes/node68.impl.ixx
        nodes/node69.ixx
        nodes/node69.impl.ixx
        nodes/node70.ixx
        nodes/node70.impl.ixx
        nodes/node71.ixx
        nodes/node71.impl.ixx
        nodes/node72.ixx
        nodes/node72.impl.ixx
        nodes/node73.ixx
        nodes/node73.impl.ixx
        nodes/node74.ixx
        nodes/node74.impl.ixx
        nodes/node75.ixx
        nodes/node75.impl.ixx
        nodes/node76.ixx
        nodes/node76.impl.ixx
        nodes/node77.ixx
        nodes/node77.impl.ixx
        nodes/node78.ixx
        nodes/node78.impl.ixx
        nodes/node79.ixx
        nodes/node79.impl.ixx
        nodes/node80.ixx
        nodes/node80.impl.ixx
        nodes/node81.ixx
        nodes/node81.impl.ixx
        nodes/node82.ixx
        nodes/node82.impl.ixx
        nodes/node83.ixx
        nodes/node83.impl.ixx
        nodes/node84.ixx
        nodes/node84.impl.ixx
        nodes/node85.ixx
        nodes/node85.impl.ixx
        nodes/node86.ixx
        nodes/node86.impl.ixx
        nodes/node87.ixx
        nodes/node87.impl.ixx
        nodes/node88.ixx
        nodes/node88.impl.ixx
        nodes/node89.ixx
        nodes/node89.impl.ixx
        nodes/node90.ixx
        nodes/node90.impl.ixx
        nodes/node91.ixx
        nodes/node91.impl.ixx
        nodes/node92.ixx
        nodes/node92.impl.ixx
        nodes/node93.ixx
        nodes/node93.impl.ixx
        nodes/node94.ixx
        nodes/node94.impl.ixx
        nodes/node95.ixx
        nodes/node95.impl.ixx
        nodes/node96.ixx
        nodes/node96.impl.ixx
        nodes/node97.ixx
        nodes/node97.impl.ixx
        nodes/node98.ixx
        nodes/node98.impl.ixx
        nodes/node99.ixx
        nodes/node99.impl.ixx
)

target_generate_di_src(di_bench_compile99
    GRAPH_MODULE di.bench.compile.graph
    GRAPH_TYPE   di::Graph<di::bench::compile::Graph>
    NODES
        node1    di.bench.compile.node1
        node2    di.bench.compile.node2
        node3    di.bench.compile.node3
        node4    di.bench.compile.node4
        node5    di.bench.compile.node5
        node6    di.bench.compile.node6
        node7    di.bench.compile.node7
        node8    di.bench.compile.node8
        node9    di.bench.compile.node9
        node10   di.bench.compile.node10
        node11   di.bench.compile.node11
        node12   di.bench.compile.node12
        node13   di.bench.compile.node13
        node14   di.bench.compile.node14
        node15   di.bench.compile.node15
        node16   di.bench.compile.node16
        node17   di.bench.compile.node17
        node18   di.bench.compile.node18
        node19   di.bench.compile.node19
        node20   di.bench.compile.node20
        node21   di.bench.compile.node21
        node22   di.bench.compile.node22
        node23   di.bench.compile.node23
        node24   di.bench.compile.node24
        node25   di.bench.compile.node25
        node26   di.bench.compile.node26
        node27   di.bench.compile.node27
        node28   di.bench.compile.node28
        node29   di.bench.compile.node29
        node30   di.bench.compile.node30
        node31   di.bench.compile.node31
        node32   di.bench.compile.node32
        node33   di.bench.compile.node33
        node34   di.bench.compile.node34
        node35   di.bench.compile.node35
        node36   di.bench.compile.node36
        node37   di.bench.compile.node37
        node38   di.bench.compile.node38
        node39   di.bench.compile.node39
        node40   di.bench.compile.node40
        node41   di.bench.compile.node41
        node42   di.bench.compile.node42
        node43   di.bench.compile.node43
        node44   di.bench.compile.node44
        node45   di.bench.compile.node45
        node46   di.bench.compile.node46
        node47   di.bench.compile.node47
        node48   di.bench.compile.node48
        node49   di.bench.compile.node49
        node50   di.bench.compile.node50
        node51   di.bench.compile.node51
        node52   di.bench.compile.node52
        node53   di.bench.compile.node53
        node54   di.bench.compile.node54
        node55   di.bench.compile.node55
        node56   di.bench.compile.node56
        node57   di.bench.compile.node57
        node58   di.bench.compile.node58
        node59   di.bench.compile.node59
        node60   di.bench.compile.node60
        node61   di.bench.compile.node61
        node62   di.bench.compile.node62
        node63   di.bench.compile.node63
        node64   di.bench.compile.node64
        node65   di.bench.compile.node65
        node66   di.bench.compile.node66
        node67   di.bench.compile.node67
        node68   di.bench.compile.node68
        node69   di.bench.compile.node69
        node70   di.bench.compile.node70
        node71   di.bench.compile.node71
        node72   di.bench.compile.node72
        node73   di.bench.compile.node73
        node74   di.bench.compile.node74
        node75   di.bench.compile.node75
        node76   di.bench.compile.node76
        node77   di.bench.compile.node77
        node78   di.bench.compile.node78
        node79   di.bench.compile.node79
        node80   di.bench.compile.node80
        node81   di.bench.compile.node81
        node82   di.bench.compile.node82
        node83   di.bench.compile.node83
        node84   di.bench.compile.node84
        node85   di.bench.compile.node85
        node86   di.bench.compile.node86
        node87   di.bench.compile.node87
        node88   di.bench.compile.node88
        node89   di.bench.compile.node89
        node90   di.bench.compile.node90
        node91   di.bench.compile.node91
        node92   di.bench.compile.node92
        node93   di.bench.compile.node93
        node94   di.bench.compile.node94
        node95   di.bench.compile.node95
        node96   di.bench.compile.node96
        node97   di.bench.compile.node97
        node98   di.bench.compile.node98
        node99   di.bench.compile.node99
)
