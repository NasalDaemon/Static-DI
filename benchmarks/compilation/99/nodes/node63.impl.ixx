module di.bench.compile.node63:impl;
import di.bench.compile.node63;

namespace di::bench::compile {

template<class Context>
int Node63::Node<Context>::impl(trait::Trait63::get) const
{
    return i + getNode(trait::trait62).get();
}

}