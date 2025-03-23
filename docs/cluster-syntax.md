# Cluster syntax

Clusters are the backbone of Static-DI: without them, nodes cannot have their dependencies on other nodes satisfied. They are classes which hold the state of a set of nodes, while describing their dependencies on each other. A cluster in turn forms a greater "node", or component, composed of its constituent nodes.

With Static-DI, one can use a special DSL (Domain Specific Language) called "dig", to define clusters (and [traits](trait-syntax.md)). Dig is designed to help visualise the graph of dependencies between nodes better than a purely textual code-based description could. The clusters are to be defined in separate files next to your other source files with the extension `.ixx.dig` (module) or `.hxx.dig` (header).

The provided CMake functions `target_generate_di_modules()` and `target_generate_di_headers()` automatically generate `.ixx` or `.hxx` files from the `.ixx.dig` and `.hxx.dig` files, found recursively in the source directory, and adds them to the target. In the case of a generated `.hxx` file, each resulting include path is identical to the include path of the respective `.hxx.dig` file.

## Cluster `.dig` file template

### Header includes and module imports

Header includes and module imports should be listed at the top of the file, [see here](dig-files.md) for further information.

### Defining a cluster

NOTE: when defining a cluster, it must either be wrapped in a namespace, which cannot be anonymous, or given a qualified name including the namespace. Clusters take the following form (simplified):

```
namespace <fully::qualified::namespace> {

cluster <cluster-name> [<cluster-annotations>...]
{
    <"node":
    : <node-name> = <node-type>
    >...

    <either "alias":
        : using <alias-name> = <trait-type>
     or "connection-block":
        <"connection-block-tag":
        : [<trait-type or alias-name>]
        >
        <"connection":
        : <node-name>... <arrow> <node-name>...
        >...
    >...
}

}
```

Which is exemplified in a concrete way by the following cluster `my::name_space::FruitSalad`, which implements `my::trait::FruitSalad` and `my::trait::ChopFruit`:
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
}

}
```

### Qualifying cluster namespace inline

It is possible to avoid wrapping the cluster in a namespace, like:
```cpp
cluster my::name_space::FruitSalad { /* ... */ }
```

**NOTE:** namespaces are not elided when defining a cluster. The combination of a wrapping namespace and a qualified cluster name always results in appending the namespace of the qualified cluster to the wrapping namespace. For example:
```cpp
namespace first::second {

cluster second::third::Cluster { /* ...*/ }

}
```
Results in `first::second::second::third::Cluster`, **not** `first::second::third::Cluster`.

### Cluster type annotations

To make use of the `Context` and `Root` types inside a cluster, the optional type annotations should be listed after the cluster name with square-brackets:
```cpp
cluster ClusterWithTypes [Context, Root]
{
    apple = Root::Apple
}
```
Alternative names corresponding to the context and the root types can be specified as follows:
```cpp
cluster ClusterWithNamedTypes [C = Context, R = Root]
{
    apple = R::Apple
}
```
Without their respective annotations, the Context and Root type names are implementation-defined, and so should not be used inside the cluster.
