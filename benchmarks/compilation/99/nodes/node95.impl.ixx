module di.bench.compile.node95:impl;
import di.bench.compile.node95;

namespace di::bench::compile {

template<class Context>
int Node95::Node<Context>::impl(trait::Trait95::get) const
{
    return i + getNode(trait::trait94).get();
}

} // namespace di::bench::compile
