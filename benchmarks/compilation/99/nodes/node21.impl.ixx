module di.bench.compile99.node21:impl;
import di.bench.compile99.node21;

namespace di::bench::compile99 {

template<class Context>
int Node21::Node<Context>::impl(trait::Trait21::get) const
{
    return i + getNode(trait::trait20).get();
}

}
