module di.bench.compile.node15:impl;
import di.bench.compile.node15;

namespace di::bench::compile {
template<class Context>
int Node15::Node<Context>::impl(trait::Trait15::get) const
{
    return i + getNode(trait::trait14).get();
}
}
