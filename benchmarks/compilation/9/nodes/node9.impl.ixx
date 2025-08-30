module di.bench.compile9.node9:impl;
import di.bench.compile9.node9;

namespace di::bench::compile9 {

template<class Context>
int Node9::Node<Context>::impl(trait::Trait9::get) const
{
    return i + getNode(trait::trait8).get();
}

}
