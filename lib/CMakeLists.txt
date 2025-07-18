add_library(di_lib INTERFACE)
add_library(di::di ALIAS di_lib)

add_library(di_module_lib)
add_library(di::module ALIAS di_module_lib)

target_compile_features(di_lib INTERFACE cxx_std_23)
target_compile_features(di_module_lib PUBLIC cxx_std_23)

target_include_directories(di_lib INTERFACE include macros/include)

# Do not expose the underlying di header library from the module lib (apart from macros)
target_link_libraries(di_module_lib PRIVATE di_lib)
target_include_directories(di_module_lib PUBLIC macros/include)

target_generate_di_headers(di_lib INCLUDE_DIR include)

target_sources(di_module_lib
    PUBLIC FILE_SET CXX_MODULES FILES
        modules/di.ixx
)

if(DI_COMPRESS_TYPES)
    target_compile_definitions(di_module_lib PRIVATE   DI_COMPRESS_TYPES=1)
    target_compile_definitions(di_lib        INTERFACE DI_COMPRESS_TYPES=1)
endif()

if(CMAKE_CXX_MODULE_STD)
    target_compile_definitions(di_module_lib PUBLIC DI_IMPORT_STD=1)
endif()
