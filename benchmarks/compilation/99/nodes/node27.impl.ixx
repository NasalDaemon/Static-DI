module di.bench.compile.node27:impl;
import di.bench.compile.node27;

namespace di::bench::compile {

template<class Context>
int Node27::Node<Context>::impl(trait::Trait27::get) const
{
    return i + getNode(trait::trait26).get();
}

}