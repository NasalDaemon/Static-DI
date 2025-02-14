add_library(di_abc_module_lib)
add_library(di::abc::module ALIAS di_abc_module_lib)

target_link_libraries(di_abc_module_lib PUBLIC di::module)

target_generate_di_modules(di_abc_module_lib)

target_sources(di_abc_module_lib
    PUBLIC FILE_SET CXX_MODULES FILES
        abc/alice.ixx
        abc/bob.ixx
        abc/charlie.ixx
        abc/ellie.ixx
        abc/graph.ixx
    PRIVATE
        abc/alice.cpp
        abc/ellie.cpp
)
