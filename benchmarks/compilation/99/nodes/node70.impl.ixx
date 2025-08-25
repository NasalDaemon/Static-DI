module di.bench.compile.node70:impl;
import di.bench.compile.node70;

namespace di::bench::compile {

template<class Context>
int Node70::Node<Context>::impl(trait::Trait70::get) const
{
    return i + getNode(trait::trait69).get();
}

}
