module di.bench.compile99.node3:impl;
import di.bench.compile99.node3;

namespace di::bench::compile99 {

template<class Context>
int Node3::Node<Context>::impl(trait::Trait3::get) const
{
    return i + getNode(trait::trait2).get();
}

}
