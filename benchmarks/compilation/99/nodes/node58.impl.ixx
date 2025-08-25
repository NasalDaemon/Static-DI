module di.bench.compile.node58:impl;
import di.bench.compile.node58;

namespace di::bench::compile {

template<class Context>
int Node58::Node<Context>::impl(trait::Trait58::get) const
{
    return i + getNode(trait::trait57).get();
}

} // namespace di::bench::compile
