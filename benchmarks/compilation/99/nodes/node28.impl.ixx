module di.bench.compile.node28:impl;
import di.bench.compile.node28;

namespace di::bench::compile {

template<class Context>
int Node28::Node<Context>::impl(trait::Trait28::get) const
{
    return i + getNode(trait::trait27).get();
}

} // namespace di::bench::compile
