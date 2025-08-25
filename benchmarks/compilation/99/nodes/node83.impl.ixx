module di.bench.compile.node83:impl;
import di.bench.compile.node83;

namespace di::bench::compile {

template<class Context>
int Node83::Node<Context>::impl(trait::Trait83::get) const
{
    return i + getNode(trait::trait82).get();
}

} // namespace di::bench::compile
