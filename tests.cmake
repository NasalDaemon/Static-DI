if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fcolor-diagnostics -Werror -Wall -Wextra -Wpedantic")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold -stdlib=libc++ -lc++abi")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-all-candidates -fconcepts-diagnostics-depth=5")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
    # Unfortunately, GCC LTO partitioning and modules do not mix well (symbols often missing at link time)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-fat-lto-objects -flto-partition=none")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
endif()

enable_testing()

add_subdirectory(di)
add_subdirectory(tests)

# GCC PCH does not improve compile times
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_subdirectory(di/lib/pch)
    target_precompile_headers(di_abc_lib REUSE_FROM di::pch)
    target_precompile_headers(di_test_headers REUSE_FROM di::abc::lib)
endif()
