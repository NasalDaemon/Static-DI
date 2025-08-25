module di.bench.compile.node64:impl;
import di.bench.compile.node64;

namespace di::bench::compile {

template<class Context>
int Node64::Node<Context>::impl(trait::Trait64::get) const
{
    return i + getNode(trait::trait63).get();
}

} // namespace di::bench::compile
