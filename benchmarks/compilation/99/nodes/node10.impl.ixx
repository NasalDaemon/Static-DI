module di.bench.compile.node10:impl;
import di.bench.compile.node10;

namespace di::bench::compile {

template<class Context>
int Node10::Node<Context>::impl(trait::Trait10::get) const
{
    return i + getNode(trait::trait9).get();
}

}
