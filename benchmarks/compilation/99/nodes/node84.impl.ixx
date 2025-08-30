module di.bench.compile99.node84:impl;
import di.bench.compile99.node84;

namespace di::bench::compile99 {

template<class Context>
int Node84::Node<Context>::impl(trait::Trait84::get) const
{
    return i + getNode(trait::trait83).get();
}

}
