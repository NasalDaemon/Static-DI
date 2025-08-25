module di.bench.compile.node72:impl;
import di.bench.compile.node72;

namespace di::bench::compile {

template<class Context>
int Node72::Node<Context>::impl(trait::Trait72::get) const
{
    return i + getNode(trait::trait71).get();
}

} // namespace di::bench::compile
