module di.bench.compile.node92:impl;
import di.bench.compile.node92;

namespace di::bench::compile {

template<class Context>
int Node92::Node<Context>::impl(trait::Trait92::get) const
{
    return i + getNode(trait::trait91).get();
}

}
