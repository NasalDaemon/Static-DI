add_library(di_lib INTERFACE)
add_library(di::lib ALIAS di_lib)
target_include_directories(di_lib INTERFACE include macros/include)

add_library(di_module_lib)
add_library(di::module ALIAS di_module_lib)

# Do not expose the underlying di header library from the module lib
target_include_directories(di_module_lib PRIVATE include PUBLIC macros/include)

target_sources(di_module_lib
    PUBLIC FILE_SET CXX_MODULES FILES
        modules/di.ixx
)

if(CMAKE_CXX_MODULE_STD)
    target_compile_definitions(di_module_lib PUBLIC "DI_IMPORT_STD=import std" DI_STD_MODULE=1)
else()
    target_compile_definitions(di_module_lib PUBLIC "DI_IMPORT_STD=")
endif()
