module di.bench.compile.node36:impl;
import di.bench.compile.node36;

namespace di::bench::compile {

template<class Context>
int Node36::Node<Context>::impl(trait::Trait36::get) const
{
    return i + getNode(trait::trait35).get();
}

} // namespace di::bench::compile
