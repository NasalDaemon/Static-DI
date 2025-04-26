set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(DI_ENABLE_LTO "Enable LTO" TRUE)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    string(APPEND CMAKE_CXX_FLAGS " -Werror -Wall -Wextra -Wpedantic")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-ld=lld -lc++abi")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    string(APPEND CMAKE_CXX_FLAGS " "
        "-Werror -Wall -Wextra -Wpedantic "
        "-fdiagnostics-all-candidates -fconcepts-diagnostics-depth=5 "
    )
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-ld=mold")

    if(DI_ENABLE_LTO)
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "15.1.0")
            # Unfortunately, GCC 14 LTO partitioning and modules do not mix well (symbols often missing at link time)
            string(APPEND CMAKE_CXX_FLAGS
                "-flto-partition=none "
            )
        else()
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gcc-inc-lto")
            string(APPEND CMAKE_CXX_FLAGS
                "-flto-partition=cache "
                "-flto-incremental=${CMAKE_CURRENT_BINARY_DIR}/gcc-inc-lto "
            )
        endif()
    endif()
endif()
