# Static-DI: The New Standard for C++ Architecture

**Static-DI** is the C++23 dependency injection framework that gives modern projects everything they need: uncompromising runtime speed, explicit and maintainable architecture, lightning-fast modular builds, and seamless transitions between monolith and microservice—all in pure, modern C++. This is a standalone library which may be imported either as a C++20 module or a header-only library.

## Why Static-DI?

- **True Zero Overhead:** Not only at graph construction, but also at resolution and invocation. There are no vtables, heap-allocations, runtime lookups, or hidden layers—just direct, inlinable calls.
- **Blazing Build Speed:** Each node can compile independently (even when building with C++ modules), so you never pay for modularity with slow builds.
- **Architecture as Code:** Declare your dependencies and boundaries using a readable DSL. Your system’s structure is explicit, enforced, and easy to understand.
- **Testability by Default:** Swap in mocks or stubs anywhere, with full compile-time safety and zero runtime cost.
- **No Intrusive Macros or Boilerplate:** Enjoy clean, modern C++ without DI gymnastics.
- **Hybrid Static & Dynamic Dispatch:** Use static wiring everywhere for maximum performance; selectively enable runtime polymorphism only where you need it—effortlessly.
- **Monolith & Microservice, Unified:** Instantly switch any component between in-process, out-of-process, or back—at any stage. Test, scale, and adapt without friction or rewrites.
- **Thread Safety by Design:** Thread affinity is enforced at compile time, eliminating entire classes of concurrency bugs before they start.

## What Makes Static-DI Unique?

- **No More Trade-Offs:** Other DI frameworks force you to choose between performance, modularity, testability, or maintainability. Static-DI delivers all, without compromise.
- **No Hidden Complexity:** No global state, no accidental coupling, no architectural lock-in. Refactoring and onboarding become straightforward—even in large, multi-team codebases.
- **Deployment Agnostic:** Effortlessly migrate between monolith and microservices as your needs evolve, making it perfect for both greenfield and long-lived projects.
- **One Less Reason to Choose Another Language:** Static-DI erases the architectural and productivity gap that often pushes teams toward other languages for new projects. Now, you can have modern practices, safety, and velocity—right in C++. This is the framework that lets you build fast, scalable, future-proof systems—without compromise. Raise your standards for new projects: make Static-DI your foundation and build in C++.

## Compiler Support
- [x] Clang 20+
- [x] GCC 14.2+ (15+ for `import std;`)
- [x] MSVC 19 (header only)
- [ ] MSVC (modules)

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
<details>
<summary>Generating .cpp files for parallel compilation of nodes and faster incremental builds (optional)</summary>

#### Generating .cpp files

To generate {graph}.{node}.cpp files which instantiate your {app.node}:impl implementation partitions for a specified graph, use `target_generate_di_src`. As each {graph}.{node}.cpp will have visibility of all sibling nodes' module interfaces (via its injected Context), it is important for each {app.node} module interface not to define any non-template functions, leaving as much as possible of the implementation in the respective {app.node}:impl implementation partition.

By having each {app.node}:impl implementation instantiated in a separate {graph}.{node}.cpp file, it allows all listed nodes to be compiled in parallel which can greatly speed up compilation. It also means that only one {graph}.{node}.cpp file will need to be recompiled if its respective {app.node}:impl implementation changes, rather than all nodes in the graph, greatly improving incremental build times during development.
```CMake
# Consider enabling LTO for production builds so that inter-node function calls are inlined
set_property(TARGET your_modules_lib PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
# Alternatively, enable LTO for your whole project:
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Generate .cpp files for listed nodes and add them to the target
target_generate_di_src(your_modules_lib
    [ID unique]                                # needed if target_generate_di_src(...) is used more than once with the same GRAPH_MODULE
    [COMMON_MODULES module.name...]            # modules to import in all generated .cpp files for this graph
    [COMMON_HEADERS path/to/header.hpp...]     # headers to include in all generated .cpp files for this graph
    GRAPH_MODULE your.app.cluster              # module containing the root cluster within which each listed node exists
    GRAPH_TYPE   di::Graph<your::app::Cluster> # the type of the graph within which each listed node has a context
    NODES                                      # List of pairs: node.path.from.root.cluster   module.name[:impl]
        apple          your.app.apple:impl
        orange         your.app.orange         # :impl is default implementation parition name, so it can be ommitted
        path.to.pear   your.app.pear:node_impl # :node_impl parition is used instead of :impl
        all, in, one   your.app.all, your.app.in, your.app.one
        # nodes all+in+one to be instantiated in the same generated cpp
)
```
</details>
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
<summary>Generating .cpp files for parallel compilation of nodes and faster incremental builds (optional)</summary>

#### Generating .cpp files

To generate {graph.node}.cpp files which instantiate your {node}.tpp implementation files for a specified graph, use `target_generate_di_src`. As each {graph.node}.cpp will have visibility of all sibling nodes' headers (via its injected Context), it is important for each {node}.hpp not define any non-template functions, leaving as much of the implementation in the respective {node}.tpp file as possible (which should not be included in any headers).

By having each {node}.tpp implementation instantiated in a separate {graph.node}.cpp file, it allows all listed nodes to be compiled in parallel which can greatly speed up compilation. It also means that only one {graph.node}.cpp file will need to be recompiled if its respective {node}.tpp implementation changes, rather than all nodes in the graph, greatly improving incremental build times during development.
```CMake
# Consider enabling LTO for production builds so that inter-node function calls are inlined
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

## Short example

```cpp
// File: app/traits.ixx.dig
export module app.traits;

trait app::trait::Person
{
    hello()
    respond()
}
```
```cpp
// File: app/forum.ixx.dig
export module app.forum;

import app.alice;
import app.bob;
import app.traits;

cluster app::Forum
{
    alice = Alice
    bob = Bob

    [trait::Person]
    alice --> bob   // Can also simply be expressed as:
    alice <-- bob   // alice <-> bob
}
```
```cpp
// File: app/alice.ixx
export module app.alice;

import app.traits;
import di;
import std;

namespace app {

// Short-hand node with contextless state
struct Alice : di::Node
{
    // (Optional) list of traits this node depends on
    using Requires = di::Requires<trait::Person>;
    // Traits provided by this node
    using Traits = di::Traits<Alice, trait::Person>;

    void apply(this auto& self, trait::Person::hello) const
    {
        std::println("Hello from Alice! I am {} years old.", self.age);
        // Context injected in explicit object parameter `self`, gives access to other nodes
        self.getNode(trait::person).respond();
    }

    void apply(trait::Person::respond) const
    {
        std::println("Well met, I am Alice of {} years!", age);
    }

    Alice(int age) : age(age) {}
    int age; // State specific to this node
};

}
```
```cpp
// File: app/bob.ixx
export module app.bob;

import app.traits;
import di;
import std;

namespace app {

// Full node with contextful state
struct Bob
{
    template<class Context>
    struct Node : di::Node
    {
        using Requires = di::Requires<trait::Person>;
        using Traits = di::Traits<Node, trait::Person>;

        void apply(trait::Person::hello) const
        {
            std::println("Hello from Bob!");
            // Can call getNode directly, as Context is already injected into the state
            getNode(trait::person).respond();
        }

        void apply(trait::Person::respond) const
        {
            std::println("Well met, I am Bob of {} years!", age);
        }

        Bob(int age) : age(age) {}
        int age; // State specific to this node
    };
}

}
```
```cpp
// File: app/main.cpp
import di;
import app.forum;

using namespace app;

int main()
{
    di::Graph<Forum> graph{
        .alice{29},
        .bob{30},
    };
    // Graph is a single object on the stack containing all nodes
    static_assert(sizeof(graph) == 2 * sizeof(int));

    graph.alice.asTrait(trait::person).hello();
    // Output:
    // Hello from Alice! I am 29 years old.
    // Well met, I am Bob of 30 years!

    graph.bob.asTrait(trait::person).hello();
    // Output:
    // Hello from Bob! I am 30 years old.
    // Well met, I am Alice of 29 years!

    return 0;
}
```

### Documentation and Examples
- [Example project using modules](docs/modules-example.md)
- [Defining a node](docs/node-structure.md)
- [DIG cluster: syntax](docs/cluster-syntax.md)
- [DIG domain: scalable clusters](docs/domain-syntax.md)
- [DIG trait: syntax](docs/trait-syntax.md)
- [Embedding DIG into source files](docs/dig-embed.md)
- [Selective runtime polymorphism](docs/runtime-polymorphism.md)
