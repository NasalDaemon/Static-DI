module di.bench.compile.node11:impl;
import di.bench.compile.node11;

namespace di::bench::compile {
template<class Context>
int Node11::Node<Context>::impl(trait::Trait11::get) const
{
    return i + getNode(trait::trait10).get();
}
}
