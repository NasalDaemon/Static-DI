module di.bench.compile.node75:impl;
import di.bench.compile.node75;

namespace di::bench::compile {

template<class Context>
int Node75::Node<Context>::impl(trait::Trait75::get) const
{
    return i + getNode(trait::trait74).get();
}

}