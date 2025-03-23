# Static-DI
Dependency injection framework without the need for runtime dispatch, virtual calls, or heap allocations. Standalone C++23 library which may be imported as a C++20 module. Designed to be used in resource constrained environments such as embedded systems, or where high performance is absolutely paramount and cannot be compromised.

## Compiler Support
- [x] Clang 20+
- [x] GCC 14.2+ (15+ for `import std;`)
- [ ] MSVC

## How to use in your project
Add the following to your CMake, which imports the code for the latest release into your project.
```CMake
include(FetchContent)
FetchContent_Declare(di URL https://github.com/NasalDaemon/Static-DI/archive/refs/heads/latest.tar.gz)
FetchContent_MakeAvailable(di) # makes available di::di and di::module
```
<details>
<summary>Modules</summary>

### Modules
You can link the modularized library (so you can `import di;`), with
```CMake
target_link_library(your_modules_lib PUBLIC di::module)
```
To generate module files from the Static-DI DSL (aka dig), use `target_generate_di_modules`.
```CMake
target_generate_di_modules(your_modules_lib
    [MODULE_DIR rel/path=""]
    [GLOB rel/path...]  # explicitly list dirs to search for .ixx.dig files
    [FILES rel/path...] # explicitly list .ixx.dig files
    [EMBED rel/path...] # explicitly list files with embedded dig
)
```
It generates .ixx modules from .ixx.dig files, and .ixx modules from any files listed in EMBED. All generated modules are added to the target.
</details>
<details>
<summary>Headers</summary>

### Headers
You can link the header library (so you can `#include <di/di.hpp>`), with
```CMake
target_link_library(your_headers_lib PUBLIC di::di)
```
To generate header files from the Static-DI DSL (aka dig), use `target_generate_di_headers`.
```CMake
target_generate_di_headers(your_headers_lib
    [INCLUDE_DIR rel/path=""]
    [GLOB rel/path...]  # explicitly list dirs to search for .hxx.dig files
    [FILES rel/path...] # explicitly list .hxx.dig files
    # explicitly list files with embedded dig
    [EMBED rel/input/path full/include/header.hxx]...
)
```
It generates .hxx headers from .hxx.dig files, and header files from any files listed in EMBED. All files generated from .hxx.dig are added to the target with the same include path as the input .hxx.dig files. All generated headers from embed files can be included with `#include "full/include/header.hxx"`
<details>
<summary>Generating .cpp files for parallel compilation of nodes and faster incremental builds</summary>

#### Generating .cpp files

To generate {graph.node}.cpp files which instantiate your {node}.tpp implementation files for a specified graph, use `target_generate_di_src`. As each {graph.node}.cpp will have visibility of all other nodes' headers (via the parent cluster header which injects its Context), it is important for each {node}.hpp not to include the definitions of non-template functions, leaving as much of the implementation in the respective {node}.tpp file as possible (which should not be included in any headers).

By having each {node}.tpp implementation instantiated in a separate {graph.node}.cpp file, it allows all listed nodes to be compiled in parallel which can greatly speed up compilation. It also means that only one {graph.node}.cpp file will need to be recompiled if its respective {node}.tpp implementation changes, rather than all nodes in the graph, greatly improving incremental build times during development.
```
# Enable LTO so that inter-node function calls are inlined
set_property(TARGET your_headers_lib PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
# Alternatively, enable LTO for your whole project:
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Generate .cpp files for listed nodes and add them to the target
target_generate_di_src(your_headers_lib
    [UNITY]                                    # instantiate all nodes in a single .cpp file
    [ID unique]                                # needed if target_generate_di_src(...) is used more than once with the same GRAPH_HEADER
    [COMMON_HEADERS path/to/header.hpp...]     # other headers to include in all generated .cpp files
    GRAPH_HEADER your/app/cluster.hxx          # header containing the root cluster within which each listed node exists
    GRAPH_TYPE   di::Graph<your::app::Cluster> # the type of the graph within which each listed node has a context
    NODES                                      # List of pairs: node.path.from.root.cluster path/to/impl.tpp
        apple          your/app/apple.tpp
        orange         your/app/orange.tpp
        path.to.pear   your/app/pear.tpp
        all, in, one   your/app/all.tpp, your/app/in.tpp, your/app/one.tpp
        # nodes all+in+one to be instantiated in the same generated cpp
)
```
</details>
</details>

### Documentation and Examples
- [Example project using modules](docs/modules-example.md)
- [Defining a node](docs/node-structure.md)
- [DIG cluster syntax](docs/cluster-syntax.md)
- [DIG trait syntax](docs/trait-syntax.md)
- [Embedding DIG into source files](docs/dig-embed.md)
