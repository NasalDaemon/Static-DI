module di.bench.compile.node98:impl;
import di.bench.compile.node98;

namespace di::bench::compile {

template<class Context>
int Node98::Node<Context>::impl(trait::Trait98::get) const
{
    return i + getNode(trait::trait97).get();
}

} // namespace di::bench::compile
