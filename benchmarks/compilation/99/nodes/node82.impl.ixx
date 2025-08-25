module di.bench.compile.node82:impl;
import di.bench.compile.node82;

namespace di::bench::compile {

template<class Context>
int Node82::Node<Context>::impl(trait::Trait82::get) const
{
    return i + getNode(trait::trait81).get();
}

} // namespace di::bench::compile
