add_library(di_lib INTERFACE)
add_library(di::di ALIAS di_lib)

add_library(di_module_lib)
add_library(di::module ALIAS di_module_lib)

target_compile_features(di_lib INTERFACE cxx_std_23)
target_compile_features(di_module_lib PUBLIC cxx_std_23)

target_include_directories(di_lib INTERFACE include macros/include)
# Do not expose the underlying di header library from the module lib
target_include_directories(di_module_lib PRIVATE include PUBLIC macros/include)

target_sources(di_module_lib
    PUBLIC FILE_SET CXX_MODULES FILES
        modules/di.ixx
)

if(DI_DECOMPRESS_TYPES)
    target_compile_definitions(di_module_lib PUBLIC    DI_DECOMPRESS_TYPES=1)
    target_compile_definitions(di_lib        INTERFACE DI_DECOMPRESS_TYPES=1)
endif()

if(CMAKE_CXX_MODULE_STD)
    target_compile_definitions(di_module_lib PUBLIC DI_STD_MODULE=1)
endif()
