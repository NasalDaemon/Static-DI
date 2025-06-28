# Selective Runtime Polymorphism

In large-scale C++ systems, balancing static structure with runtime flexibility is crucial—especially when migrating from a modular monolith toward microservices, or when you need to swap implementations on the fly (for testing, mocking, or hot-reloading). Traditional C++ dependency injection is either fully static (compile-time) or fully dynamic (runtime), but rarely both in a controlled, type-safe way.

`di::Union` and `di::Virtual` exist to bridge this gap. They let you introduce runtime polymorphism *only where you need it*, keeping the rest of your dependency graph static and efficient. With these tools, you can:

- **Select implementations at runtime** (e.g., for plugins, mocks, or feature toggles)
- **Hot-swap nodes** without restarting your application
- **Compose graphs that are mostly static** (for performance and clarity), but with dynamic "seams" where flexibility is required

This selective approach enables you to build systems that are both robust and adaptable—well-suited for architectures that need to scale from monoliths to microservices, or that require live reconfiguration and testing without sacrificing type safety or performance.

## `di::Union`

`di::Union<Node, Nodes...>` (see [union.hpp](../lib/include/di/union.hpp)) is to be used if the full set of possible nodes implementing an intersection of traits is known at compile-time. When the graph is constructed, a single implementation is constructed. This remains active until an alternative implementation from the list is emplaced during runtime, or the graph is destructed.

The memory layout of `di::Union<Nodes...>` is much like `std::variant<Ts...>`, where the state of the active alternative is stored on the stack alongside its respective index. Since only one node alternative is active at any one time, a `union` is effectively used to host the state. When a trait method is invoked, the only extra runtime cost is a branch to pick the active node's implementation given the active index. Due to the list of alternatives being known at compile-time, it presents opportunities for the compiler to optimise invocations, including inlining. Nodes in a `di::Union` are also able to retain compile-time thread-affinity protection where it is employed.

Only methods calls from external nodes into the `di::Union` incur the extra runtime cost. Any methods invoked from within the nodes hosted by the `di::Union` incur no extra runtime cost, including calls to methods external to the `di::Union`.

### Examples

```cpp
export module app.cluster;

import app.traits;
import app.farmer; // not shown
export import app.cow;
export import app.sheep;

cluster app::Cluster
{
    farmer = Farmer
    animal = di::Union<Cow, Sheep>

    [trait::Animal]
    famer --> animal
}
```
```cpp
export module app.traits;

import std;

trait app::trait::Animal
{
    speak() const -> std::string
}
```
```cpp
export module app.cow;

import app.traits;
import di;

namespace app {

struct Cow : di::Node
{
    using Traits = di::Traits<Cow, trait::Animal>;

    std::string apply(trait::Animal::speak) const { return happy ? "moo" : "mmmooooo!"; }

    explicit Cow(bool happy) : happy(happy) {}
    bool happy;
};

}
```
```cpp
export module app.sheep;

import app.traits;
import di;

namespace app {

struct Sheep : di::Node
{
    using Traits = di::Traits<Sheep, trait::Animal>;

    std::string apply(trait::Animal::speak) const { return "baa!"; }
};

}
```

```cpp
import app.cluster;
import std;

enum Animal
{
    Cow, Sheep
};

struct Args
{
    Animal animal;
};

Args parseArgs(int argc, char** argv);

int main(int argc, char** argv)
{
    Args args = parseArgs(argc, argv);

    di::Graph<app::Cluster> graph{
        .animal{di::withFactory,
            [&](auto construct) {
                if (args.animal == Animal::Cow)
                    return construct(std::in_place_type<Cow>, true);
                else
                    // Can use index
                    return construct(std::in_place_index<1 /* Sheep */>);
            }}
    };

    std::println("Animal says {}", graph.farmer->getNode(trait::animal).speak());

    // Change impl at runtime by index
    graph.animal->emplace<0>(false);
    std::println("Unhappy cow says {}", graph.farmer->getNode(trait::animal).speak());
    // Change impl at runtime by type
    graph.animal->emplace<Sheep>();
    std::println("Sheep says {}", graph.farmer->getNode(trait::animal).speak());
}
```

## `di::Virtual`

`di::Virtual<Interfaces...>` (see [virtual.hpp](../lib/include/di/virtual.hpp)) is to be used to host an open-set of node implementations of the given `Interfaces` (which derive from `di::INode`). This is the most flexible option, as any implementation of the interfaces can be used, even if it is not known when compiling the graph. As with `di::Union` when the graph is constructed, a single implementation is constructed. This remains active until an alternative implementation of the interface is emplaced during runtime, or the graph is destructed. Unlike static nodes, nodes deriving from `di::INode` also have the ability to swap themselves out with another implementation and handover as they see fit.

The memory layout of `di::Virtual<Interfaces...>` is much like `std::tuple<Interfaces*...>`, where the `Interfaces*` are pointers to the heap-allocated implementation. When a trait method is invoked, the interface that satisfies the trait is found from the list (as long as there is no ambiguity) and the method is called on that interface. Since the trait methods are typically implemented as virtual overrides of the interface methods, one vtable lookup is to expected per method call, much like a standard abstract class. In the case of a virtual override, it is harder for the compiler to optimise the call or inline it, although with judicious use of the `final` keyword and with whole-program or link-time-optimisation it may be possible for the compiler to devirtualise the call. Nodes hosted in a `di::Virtual` are not able to retain compile-time thread-affinity protection where it is employed, as the types needed to enforce this are erased.

Only method calls from external nodes into nodes hosted by `di::Virtual` incur the extra runtime cost. Any methods invoked from within the nodes hosted by the `di::Virtual` incur no extra runtime cost, including calls to methods external to the `di::Virtual`.

**NOTE:** It is possible to make a `di::Virtual` one of the alternatives in a `di::Union`, offering the best of both worlds. One can specify the static nodes that are most well-known or requiring the highest performance as alternatives in the `di::Union`, and the `di::Virtual` alternative can be used as a fallback for everything else. Invoking a method on the fallback `di::Virtual` would then of course require both a branch (`di::Union` active node selection) as well as a vtable lookup (`di::Virtual` method dispatch).

### `di::Adapt` and `di::Box`: hosting static nodes in a `di::Virtual`

By using `di::Adapt<StaticNode, InterfaceFacade>`, it is possible to adapt a static node that doesn't derive from `di::INode` or use virtual overrides, so that it can be hosted by a `di::Virtual`. The second template parameter is a facade, implementing the `Interface` required by the hosting `di::Virtual<Interface>`, which invokes the respective trait methods on the static node like a proxy. The static node, being static and having no knowledge of being dynamic, is not capable of swapping itself out, but the facade can swap the implementation of the `di::Virtual`, as it is a dynamic node. The static node can still call methods external to `di::Virtual` as per usual with no extra vtable lookup.

With `di::Box<StaticNode, InFacade, OutFacade = void, OutInterfaces...>`, one adaps the given static node but also adapts all external dependencies behind another `OutFacade`, implementing `OutInterfaces...`. This means that the static node must pay a virtual dispatch also in all its calls to its dependencies, which are entirely abstracted behind the `OutFacade`. Using `di::Box`, one can instantiate a completely isolated implementation of the interfaces from which `InFacade` derives, completely agnostic to the context of the hosting `di::Virtual`.

### Examples

```cpp
export module app.cluster;

import app.traits;
import app.farmer; // not shown
export import app.animal;

cluster app::Cluster
{
    farmer = Farmer
    animal = di::Virtual<IAnimal>

    [trait::Animal]
    famer --> animal
}
```
```cpp
export module app.traits;

import std;

trait app::trait::Animal
{
    speak() const -> std::string
    evolve()
}
```
```cpp
export module app.animal;

import app.traits;
import di;
import std;

namespace app {

struct IAnimal : di::INode
{
    using Traits = di::Traits<IAnimal, trait::Animal>;

    virtual std::string apply(trait::Animal::speak) const = 0;
    virtual void apply(trait::Animal::evolve) const = 0;
};

}
```
```cpp
export module app.cow;

import app.animal;
import app.traits;
import di;

namespace app {

struct Cow final : IAnimal
{
    std::string apply(trait::Animal::speak) const override
    {
        return happy ? "moo" : "mmmooooo!";
    }

    void apply(trait::Animal::evolve) override
    {
        // Cows do not evolve
    }

    explicit Cow(bool happy) : happy(happy) {}
    bool happy;
};

}
```
```cpp
export module app.sheep;

import app.animal;
import app.traits;
import app.goat;
import di;

namespace app {

struct Sheep final : IAnimal
{
    std::string apply(trait::Animal::speak) const override
    {
        return black ? "barbar" : "baa!";
    }

    void apply(trait::Animal::evolve) override
    {
        auto handle = exchangeImpl<Goat>();

        // This node is still alive in a detached state
        // It can call into itself and other nodes, but other nodes can't reach it
        std::println(asTrait(trait::animal).speak());
        // prints: barbar or baa!

        std::println(handle.getNext().asTrait(trait::animal).speak());
        // prints: meeh!
    }

    Sheep(bool black) : black(black) {}
    bool black;
};

}
```
```cpp
export module app.goat;

import app.animal;
import app.traits;
import app.cow;
import di;

namespace app {

struct Goat final : IAnimal
{
    std::string apply(trait::Animal::speak) const override { return "meeh!"; }

    void apply(trait::Animal::evolve) override
    {
        auto handle = exchangeImpl<Cow>(false);

        // This node is still alive in a detached state
        // It can call into itself and other nodes, but other nodes can't reach it
        std::println(asTrait(trait::animal).speak());
        // prints: meeh!

        std::println(handle.getNext().asTrait(trait::animal).speak());
        // prints: mmmooooo!
    }
};

}
```
```cpp
export module app.fox;

import app.traits;
import di;

namespace app {

// Fox is a static node which we can adapt to IAnimal using di::Adapt<Fox, IAnimalFacade>
struct Fox : di::Node
{
    using Traits = di::Traits<Fox, trait::Animal>;

    std::string apply(trait::Animal::speak) const override { return "yip"; }

    void apply(trait::Animal::evolve)
    {
        // static node can not swap itself out (alas, `evolve` shouldn't really be a method in the trait)
    }
};

}
```
```cpp
export module app.animal.facade;

import app.animal;

namespace app {

struct IAnimalFacade final : IAnimal
{
    // Forward to adapted node
    std::string apply(trait::Animal::speak) const override
    {
        return getNode(trait::animal).speak();
    }

    // As a dynamic node, this facade can swap the implementation hosted by di::Virtal
    void apply(trait::Animal::evolve) override
    {
        auto handle = exchangeImpl<Cow>(true);

        // This node is still alive in a detached state
        // It can call into itself and other nodes, but other nodes can't reach it
        std::println(asTrait(trait::animal).speak());
        // prints: yip! (assuming it adapted Fox)

        std::println(handle.getNext().asTrait(trait::animal).speak());
        // prints: moo!
    }
};

}
```
```cpp
import app.animal.facade;
import app.cluster;
import app.cow;
import app.sheep;
import app.fox;
import std;

enum Animal
{
    Cow, Sheep
};

struct Args
{
    Animal animal;
};

Args parseArgs(int argc, char** argv);

using namespace app;

int main(int argc, char** argv)
{
    Args args = parseArgs(argc, argv);

    di::Graph<app::Cluster> graph{
        .animal{di::withFactory,
            [&](auto construct) {
                if (args.animal == Animal::Cow)
                    return construct(std::in_place_type<Cow>, true);
                else
                    return construct(std::in_place_type<Sheep>);
            }}
    };

    auto animal = graph.farmer.getNode(trait::animal); // equivalent to graph.animal.asTrait(trait::animal)
    std::println("Animal says {}", animal.speak());

    graph.animal->emplace<Sheep>(true);
    std::println("Black sheep says {}", animal.speak());

    // Sheep hotswaps with goat
    animal.evolve();
    std::println("Goat says {}", animal.speak());
    // Goat hotswaps with sad cow
    animal.evolve();
    std::println("Sad cow says {}", animal.speak());

    // Farmer's view of animal is always up-to-date
    // The `animal` variable is just a non-owning trait view of the di::Virtual node in the graph
    std::println("Sad cow says {}", graph.farmer.getNode(trait::animal).speak());

    graph.animal->emplace<di::Adapt<Fox, IAnimalFacade>>()
    std::println("Fox says {}", animal.speak());
    // IAnimalFacade hotswaps with happy cow
    animal.evolve();
    std::println("Cow says {}", animal.speak());
}
```
