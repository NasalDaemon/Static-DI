module di.bench.compile.node99:impl;
import di.bench.compile.node99;

namespace di::bench::compile {
template<class Context>
int Node99::Node<Context>::impl(trait::Trait99::get) const
{
    return i + getNode(trait::trait98).get();
}
}
