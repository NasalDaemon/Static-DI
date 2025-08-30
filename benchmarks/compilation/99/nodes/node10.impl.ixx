module di.bench.compile99.node10:impl;
import di.bench.compile99.node10;

namespace di::bench::compile99 {

template<class Context>
int Node10::Node<Context>::impl(trait::Trait10::get) const
{
    return i + getNode(trait::trait9).get();
}

}
