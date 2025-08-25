module di.bench.compile.node26:impl;
import di.bench.compile.node26;

namespace di::bench::compile {

template<class Context>
int Node26::Node<Context>::impl(trait::Trait26::get) const
{
    return i + getNode(trait::trait25).get();
}

} // namespace di::bench::compile
