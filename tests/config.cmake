set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    string(APPEND CMAKE_CXX_FLAGS " -Werror -Wall -Wextra -Wpedantic")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-ld=lld -lc++abi")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    string(APPEND CMAKE_CXX_FLAGS " "
        "-Werror -Wall -Wextra -Wpedantic "
        "-fdiagnostics-all-candidates -fconcepts-diagnostics-depth=5 "
        # Unfortunately, GCC LTO partitioning and modules do not mix well (symbols often missing at link time)
        "-fno-fat-lto-objects -flto-partition=none "
    )
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-ld=mold")
endif()
