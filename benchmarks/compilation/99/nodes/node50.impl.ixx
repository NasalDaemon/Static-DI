module di.bench.compile.node50:impl;
import di.bench.compile.node50;

namespace di::bench::compile {

template<class Context>
int Node50::Node<Context>::impl(trait::Trait50::get) const
{
    return i + getNode(trait::trait49).get();
}

} // namespace di::bench::compile
