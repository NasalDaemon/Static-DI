module di.bench.compile.node90:impl;
import di.bench.compile.node90;

namespace di::bench::compile {

template<class Context>
int Node90::Node<Context>::impl(trait::Trait90::get) const
{
    return i + getNode(trait::trait89).get();
}

} // namespace di::bench::compile
