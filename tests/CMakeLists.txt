find_package(doctest REQUIRED)

add_subdirectory(abc)
add_subdirectory(abc/modules)

add_library(di_test_main test_main.cpp)
add_library(di::test ALIAS di_test_main)
target_link_libraries(di_test_main PUBLIC doctest::doctest)

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
target_sources(di_test_module
    PUBLIC FILE_SET CXX_MODULES FILES
        repeater/test_node.ixx
        thread/poster.ixx
        thread/test_node.ixx
)
target_generate_di_modules(di_test_module GLOB repeater thread)

target_link_libraries(di_test_headers PUBLIC di::abc::lib di::test)
target_link_libraries(di_test_module PUBLIC di::abc::module di::test)

set_property(TARGET di_test_headers PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
set_property(TARGET di_test_module PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

add_test(di_test_headers di_test_headers)
add_test(di_test_module di_test_module)
