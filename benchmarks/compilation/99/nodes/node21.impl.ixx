module di.bench.compile.node21:impl;
import di.bench.compile.node21;

namespace di::bench::compile {

template<class Context>
int Node21::Node<Context>::impl(trait::Trait21::get) const
{
    return i + getNode(trait::trait20).get();
}

}
