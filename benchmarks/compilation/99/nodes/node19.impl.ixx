module di.bench.compile.node19:impl;
import di.bench.compile.node19;

namespace di::bench::compile {
template<class Context>
int Node19::Node<Context>::impl(trait::Trait19::get) const
{
    return i + getNode(trait::trait18).get();
}
}
