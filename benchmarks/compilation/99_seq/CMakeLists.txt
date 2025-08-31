add_executable(di_bench_compile99_seq main.cpp)
target_link_libraries(di_bench_compile99_seq di::module)
target_generate_di_modules(di_bench_compile99_seq)

if(DI_BUILD_TESTS)
    add_test(di_bench_compile99_seq_test di_bench_compile99_seq)
endif()

if(DI_BUILD_LTO)
    set_property(TARGET di_bench_compile99_seq PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

file(GLOB NODE_IXX_FILES "nodes/*.ixx")
target_sources(di_bench_compile99_seq
    PUBLIC FILE_SET CXX_MODULES FILES
        ${NODE_IXX_FILES}
)
