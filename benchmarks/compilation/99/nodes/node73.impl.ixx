module di.bench.compile.node73:impl;
import di.bench.compile.node73;

namespace di::bench::compile {

template<class Context>
int Node73::Node<Context>::impl(trait::Trait73::get) const
{
    return i + getNode(trait::trait72).get();
}

} // namespace di::bench::compile
