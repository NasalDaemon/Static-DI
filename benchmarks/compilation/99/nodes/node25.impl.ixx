module di.bench.compile.node25:impl;
import di.bench.compile.node25;

namespace di::bench::compile {

template<class Context>
int Node25::Node<Context>::impl(trait::Trait25::get) const
{
    return i + getNode(trait::trait24).get();
}

}