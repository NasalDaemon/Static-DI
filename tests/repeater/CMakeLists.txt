add_executable(di_test_repeater main.cpp)

target_link_libraries(di_test_repeater PUBLIC di::module)

target_generate_di_modules(di_test_repeater)

target_sources(di_test_repeater
    PUBLIC FILE_SET CXX_MODULES FILES
        test_node.ixx
)
