add_executable(di_test_thread main.cpp)

target_link_libraries(di_test_thread PUBLIC di::module)

target_generate_di_modules(di_test_thread)

target_sources(di_test_thread
    PUBLIC FILE_SET CXX_MODULES FILES
        poster.ixx
        test_node.ixx
)
