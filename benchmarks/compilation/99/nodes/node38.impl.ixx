module di.bench.compile.node38:impl;
import di.bench.compile.node38;

namespace di::bench::compile {

template<class Context>
int Node38::Node<Context>::impl(trait::Trait38::get) const
{
    return i + getNode(trait::trait37).get();
}

}