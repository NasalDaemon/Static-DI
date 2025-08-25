module di.bench.compile.node34:impl;
import di.bench.compile.node34;

namespace di::bench::compile {

template<class Context>
int Node34::Node<Context>::impl(trait::Trait34::get) const
{
    return i + getNode(trait::trait33).get();
}

} // namespace di::bench::compile
