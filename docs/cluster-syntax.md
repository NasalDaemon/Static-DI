# Cluster syntax

Clusters are the backbone of Static-DI: without them, nodes cannot have their dependencies satisfied. They are classes that hold the state of a set of nodes and describe their dependencies on each other. A cluster forms a greater "node" or component, composed of its constituent nodes.

With Static-DI, you can use a special DSL (Domain Specific Language) called "dig" to define clusters (and [traits](trait-syntax.md)). Dig helps visualize the graph of dependencies between nodes better than a purely textual code-based description. Clusters are defined in separate files next to your other source files with the extension `.ixx.dig` (module) or `.hxx.dig` (header).

The provided CMake functions `target_generate_di_modules()` and `target_generate_di_headers()` automatically generate `.ixx` or `.hxx` files from the `.ixx.dig` and `.hxx.dig` files found recursively in the source directory, and add them to the target. For generated `.hxx` files, each resulting include path matches the include path of the respective `.hxx.dig` file.

## Cluster `.dig` file template

### Header includes and module imports

Header includes and module imports should be listed at the top of the file. [See here](dig-files.md) for further information.

### Defining a cluster

**Note:** When defining a cluster, it must either be wrapped in a namespace (which cannot be anonymous) or given a qualified name including the namespace. Clusters take the following form (simplified):

```
namespace <fully-qualified-namespace> {

cluster <cluster-name> [<cluster-annotations>...]
{
    <"node":
        | <node-name> = <node-type>
    >...

    <either "alias":
        | using <alias-name> = <trait-type>
     or "connection-block":
        <"connection-block-tag":
            | [<trait-type or alias-name>]
        >
        <either "sink-node":
            | <node-name>
         or "connection":
            | <node-name>... <arrow> <node-name>...
        >...
    >...
}

}
```

Which is exemplified by the following cluster `my::name_space::FruitSalad`, which implements `my::trait::FruitSalad` and `my::trait::ChopFruit`:
```cpp
namespace my::name_space {

cluster FruitSalad
{
    // List of nodes...
    apple = Apple
    banana = Banana
    cherry = Cherry
    sourCherry = Cherry
    date = Date
    elderberry = Elderberry

    // Sink trait
    [trait::Elderberry] elderberry
    // Connects all other nodes to elderberry (including the parent cluster)
    // Equivalent to:
    // [trait::Elderberry]
    // elderberry <-- .., apple, banana, cherry, sourCherry, date
    // Any sink traits (like `trait::Elderberry`) must be declared before all explicit connections
    // Sink nodes (like `elderBerry`) may not have any outgoing connections apart from the implicit
    // connections to global nodes and other sink nodes
    // Sink traits cannot be used in any further connection blocks

    // Connection block (using both arrow directions)
    [trait::Apple]
    banana --> apple
    apple <-- cherry
    // equivalent to:
    // [trait::Apple]
    // banana --> apple
    // cherry --> apple
    // where banana.getNode(trait::apple) ~= apple.asTrait(trait::apple)
    // and cherry.getNode(trait::apple) ~= apple.asTrait(trait::apple)

    // Alias
    using b = trait::Banana

    // Connection block (using alias and many-to-one)
    [b] apple, cherry --> banana
    // equivalent to:
    // [trait::Banana]
    // apple --> banana
    // cherry --> banana

    // Aliases
    using c = trait::cherry, d = trait::Date

    // Connection block (using bidirectional arrows)
    [c <-> d]
    cherry <-> date
    cherry <-- apple, banana
    apple, banana --> date
    // equivalent to:
    // [trait::Cherry]
    // cherry <-- date
    // [trait::Date]
    // cherry --> date
    // [trait::Cherry]
    // cherry <-- apple, banana
    // [trait::Date]
    // apple, banana --> date

    // Connection block (using trait renaming)
    [trait::Cherry]
    apple, banana (trait::SourCherry) --> sourCherry
    // equivalent to:
    // apple  (trait::SourCherry) --> (trait::Cherry) sourCherry
    // banana (trait::SourCherry) --> (trait::Cherry) sourCherry
    // where apple.getNode(trait::sourCherry) ~= sourCherry.asTrait(trait::cherry)
    // and banana.getNode(trait::sourCherry) ~= sourCherry.asTrait(trait::cherry)
    // This disambiguates cherry and sourCherry from the point of view of apple and banana,
    // although cherry and sourCherry are both just cherries from their own points of view

    // Connection block (using daisy-chain)
    [trait::FruitSalad]
    .. --> apple --> banana --> cherry --> sourCherry --> date
    // equivalent to:
    // [trait::FruitSalad]
    // .. --> apple
    //        apple --> banana
    //                  banana --> cherry
    //                             cherry --> sourCherry
    //                                        sourCherry --> date

    // Note: when FruitSalad is used as a node in a parent cluster,
    // trait::FruitSalad transparently connects to apple

    // Connection block (using one-to-many aka fan-out)
    [trait::ChopFruit]
    .. --> apple, banana, cherry
    .. --> date
    .. --> sourCherry
    // which implicitly generates a special intermediate node
    // `_parentRepeater0 = di::Repeater<trait::ChopFruit, 5>` with the connections:
    // [trait::ChopFruit]
    // .. --> _parentRepeater0 (di::RepeaterTrait<0>) --> apple
    //        _parentRepeater0 (di::RepeaterTrait<1>) --> banana
    //        _parentRepeater0 (di::RepeaterTrait<2>) --> cherry
    //        _parentRepeater0 (di::RepeaterTrait<3>) --> date
    //        _parentRepeater0 (di::RepeaterTrait<4>) --> sourCherry
    // where any trait::ChopFruit::method call is repeated by the repeater node
    // to apple, then banana, then cherry, then date, then sourCherry
    // as if calling for Is 0..4:
    // _parentRepeater0.getNode(di::RepeaterTrait<Is>{}).method(args...)

    // Note: when FruitSalad is used as a node in a parent cluster,
    // trait::ChopFruit transparently connects to _parentRepeater0

    // Explicitly redirecting trait to the global node
    [trait::Log]
    apple --> *
    // which resolves trait::Log to the respective global node
    // Equivalent to:
    // [trait::Log]
    // apple --> (di::Global<trait::Log>) ..
    // This is only necessary if `trait::Log` must be resolved using `getNode` instead of `getGlobal`
}

}
```

### Qualifying cluster namespace inline

You can avoid wrapping the cluster in a namespace, like:
```cpp
cluster my::name_space::FruitSalad { /* ... */ }
```

**Note:** Namespaces are not elided when defining a cluster. The combination of a wrapping namespace and a qualified cluster name always results in appending the namespace of the qualified cluster to the wrapping namespace. For example:
```cpp
namespace first::second {

cluster second::third::Cluster { /* ...*/ }

}
```
This results in `first::second::second::third::Cluster`, **not** `first::second::third::Cluster`.

### Cluster type annotations

To use the `Context`/`Root`/`Info` types inside a cluster, optional type annotations should be listed after the cluster name in square brackets:
```cpp
cluster ClusterWithTypes [Context, Root, Info]
{
    apple = Root::Apple
}
```
Alternative names corresponding to the context and the root types can be specified as follows:
```cpp
cluster ClusterWithNamedTypes [C = Context, R = Root, I = Info]
{
    apple = R::Apple
}
```
Without their respective annotations, the Context, Root, and Info type names are implementation-defined, and so should not be used inside the cluster.

## No-trait connections [~]

Use the no-trait marker `~` when you do not need a named trait/interface for a link. It models a traitless connection that stays type-safe via `di::NoTrait<NodeHandle>` at codegen time. The client will have access to all of the provider's public data and member functions without any interface constraints.

### When to use
- Internal wiring where the consumer needs the provider as-is (no interface to constrain)
- One implementation per role with no plan to swap behind a trait
- Prototyping before introducing a named trait
- Tests and examples to keep graphs concise

### When not to use
- Public or cross-cluster contracts where a stable interface matters
- Multiple interchangeable implementations selected by trait
- You need to mix different client views of the same node (use named traits)

### Syntax
#### Connection block (traitless):
```cpp
cluster MyCluster
{
    client = Client
    provider = Provider

    // No-trait connection to a node
    [~] client --> provider
    // Equivalent to:
    // [di::NoTrait<Provider>]
    // client --> provider
}
```
#### Sink declaration (collect all clients by default):
```cpp
[~] logger
// All nodes (including parent) implicitly connect to `logger` using the no-trait link
```
#### Multiple sinks in one declaration:
```cpp
[~] logger, metrics
// Multiple no-trait sinks can be declared in one line (only possible for no-trait connections)
```
#### No trait node
```cpp
struct Provider : di::Node
{
    using Traits = di::NoTraits<Provider>;
    // No traits, just public members
    void memberFunction() { /* ... */ }
};
```
#### Consumption
```cpp
// From client node
auto provider = getNode(di::noTrait<Provider>);
provider->memberFunction(); // Calls `Provider`'s member function directly
```

### Caveats:
- Cannot not mix `[~]` and a named trait for the same target node in one graph; pick one.
- No-trait with the global node (`*`) is not supported; use a named trait or `getGlobal`.

Migration tip
- If you later need a stable interface, replace `[~]` with a named trait (e.g., `[trait::X]`) and update call sites from `getNode(di::noTrait<T>)` to `getNode(trait::x)`.
