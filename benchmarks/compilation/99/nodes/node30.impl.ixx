module di.bench.compile.node30:impl;
import di.bench.compile.node30;

namespace di::bench::compile {

template<class Context>
int Node30::Node<Context>::impl(trait::Trait30::get) const
{
    return i + getNode(trait::trait29).get();
}

}
