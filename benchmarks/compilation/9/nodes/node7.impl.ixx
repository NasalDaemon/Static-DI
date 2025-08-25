module di.bench.compile.node7:impl;
import di.bench.compile.node7;

namespace di::bench::compile {

template<class Context>
int Node7::Node<Context>::impl(trait::Trait7::get) const
{
    return i + getNode(trait::trait6).get();
}

}
