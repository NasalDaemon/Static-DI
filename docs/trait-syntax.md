# Trait syntax

A trait is an interface to be presented by one node to another in order to satisfy a dependency. In a [cluster](cluster-syntax.md), all node dependencies are to be satisfied by connecting to the appropriate providing nodes by the name of the trait that they implement. As a result, on each dependent node, a `my::TraitName::method` on the providing node can be invoked via `getNode(my::traitName).method(args...)`, and a `TypeName` can be resolved via `di::ResolveTypes<Context, my::TraitName>::TypeName`. Please see the example [here](modules-example.md#sessionsixx).

With Static-DI, one can use a special DSL (Domain Specific Language) to define traits (and [clusters](cluster-syntax.md)). The DSL is designed to provide and constrain the methods that can be called from the result of `getNode(my::traitName)`, and also to list any further type requirements of each implementation. The traits are to be defined in separate files next to your other source files with the extension `.ixx.dig` (module) or `.hxx.dig` (header).

The provided CMake functions `target_generate_di_modules()` and `target_generate_di_headers()` automatically generate `.ixx` or `.hxx` files from the `.ixx.dig` and `.hxx.dig` files, found recursively in the source directory, and adds them to the target. In the case of a generated `.hxx` file, each resulting include path is identical to the include path of the respective `.hxx.dig` file.

## Trait `.dig` file template

### Header includes and module imports

Header includes and module imports should be listed at the top of the file, [see here](dig-headers.md) for further information.

### Defining a trait

NOTE: when defining a trait, it must either be wrapped in a namespace, which cannot be anonymous, or given a qualified name including the namespace.

```cpp
namespace my {

// Non-template methods are added to the `di::Implements<Impl, my::Trait>` concept,
// which is enforced in the call to `getNode` and `asTrait`.
trait Trait
{
    // `getNode(trait).method(int)` can be called in a const context
    // the return type is unconstrained, i.e. `decltype(auto)`
    // any implementation missing this overload will fail in `getNode`
    method(int i) const

    // `getNode(trait).method(double)` can be called in a mutable context
    // the return type must be convertible to `double`, which is what will be returned
    // any implementation missing this overload will fail in `getNode`
    method(double d) -> double
}

// Template methods are supported, but are not added to the `di::Implements<Impl, my::Complex>` concept,
// so their existance is *not* enforced in the call to `getNode` or `asTrait`.
// Instead, compilation only fails when the method is called and it is not implemented or conforming.
trait Complex
{
    // `getNode(complex).unconstrained(...)` can be called with any set of arguments
    // the return type is unconstrained, i.e. decltype(auto)
    // any non-conformant implementation will compile until the method is called
    unconstrained(...)

    // `getNode(complex).complex<0>(0, 0, 0, 0, 0.0)` can be called
    // the return type must be exactly `double`
    // any non-conformant implementation will compile until the method is called
    template<std::size_t I, class T, class U, std::same_as<int> Int>
    complex(T t, U&& u, Int i, auto a, std::same_as<double> auto d) -> std::same_as<double>
}

// If requirements on the trait types need to be enforced
trait Trait2 [Types: T]
{
    type Int
    // equivalent to:
    // requires typename T::Int

    // Can add extra constraint to Int (using name specified after `Types:`)
    requires std::integral<typename T::Int>
}

// If requirements on the root types need to be enforced
trait Trait3 [Root: R]
{
    root Int
    // equivalent to:
    // requires typename R::Int

    // Can add extra constraint to Int (using name specified after `Root:`)
    requires std::integral<typename R::Int>
}

// It is not necessary to rename Types or Root
trait Trait4 [Types, Root]
{
    requires std::integral<typename Types::Int>
    requires std::integral<typename Root::Int>

    // Can add arbitrary concept constraints, including:
    // `impl_` for mutable implementation, `const_` for const implementation

    // Added to di::Implements<Impl, Trait4>:
    // mutable implementation have have mutableMethod(std::string)
    requires (std::string s) {
        impl_.mutableMethod(s)
    }

    // Added to di::Implements<Impl, Trait4>
    // const implementation have have constMethod(int)
    requires const_.constMethod(0)
}

// `Trait` interface is provided and enforced, but must be resolved using name `AltTrait`.
// Useful for disambiguation between two dependencies implementing the same trait.
trait AltTrait = Trait

// `Trait` and `Complex` interfaces are both provided and enforced.
// A node implementing `TraitAndComplex` also implicitly implements `Trait` and `Complex`
// and can be resolved as such, as long as the `Trait`/`Complex` connection exists between nodes.
trait TraitAndComplex = Trait + Complex

}
```

### Qualifying trait namespace inline

It is possible to avoid wrapping the trait in a namespace, like:
```cpp
trait my::Trait
{
    // ...
}
```

**NOTE:** namespaces are not elided when defining a trait. The combination of a wrapping namespace and a qualified trait name always results in appending the namespace of the qualified trait to the wrapping namespace. For example:
```cpp
namespace first::second {

trait second::third::Trait { /* ...*/ }

}
```
Results in `first::second::second::third::Trait`, **not** `first::second::third::Trait`.
