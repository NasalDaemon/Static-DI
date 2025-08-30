module di.bench.compile99.node29:impl;
import di.bench.compile99.node29;

namespace di::bench::compile99 {

template<class Context>
int Node29::Node<Context>::impl(trait::Trait29::get) const
{
    return i + getNode(trait::trait28).get();
}

}
