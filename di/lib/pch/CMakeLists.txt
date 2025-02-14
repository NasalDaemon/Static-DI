add_library(di_pch_lib STATIC blank.cpp)
add_library(di::pch ALIAS di_pch_lib)

target_link_libraries(di_pch_lib PUBLIC di_lib)

target_precompile_headers(di_pch_lib PUBLIC
    [["di/di.hpp"]]
)
