module di.bench.compile.node3:impl;
import di.bench.compile.node3;

namespace di::bench::compile {

template<class Context>
int Node3::Node<Context>::impl(trait::Trait3::get) const
{
    return i + getNode(trait::trait2).get();
}

}
