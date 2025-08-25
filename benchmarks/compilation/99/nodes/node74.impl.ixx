module di.bench.compile.node74:impl;
import di.bench.compile.node74;

namespace di::bench::compile {

template<class Context>
int Node74::Node<Context>::impl(trait::Trait74::get) const
{
    return i + getNode(trait::trait73).get();
}

} // namespace di::bench::compile
