module di.bench.compile.node53:impl;
import di.bench.compile.node53;

namespace di::bench::compile {

template<class Context>
int Node53::Node<Context>::impl(trait::Trait53::get) const
{
    return i + getNode(trait::trait52).get();
}

}
