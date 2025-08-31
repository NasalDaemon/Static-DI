load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "headers",
    includes = [
        "include",
        "macros/include",
    ],
    defines = ["DI_COMPRESS_TYPES=1"],
)

cc_library(
    name = "module",
    features = ["cpp_modules"],
    implementation_deps = [":headers"],
    includes = ["macros/include"],
    module_interfaces = ["modules/di.ixx"],
    local_defines = ["DI_COMPRESS_TYPES=1"],
)
