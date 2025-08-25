module di.bench.compile.node86:impl;
import di.bench.compile.node86;

namespace di::bench::compile {

template<class Context>
int Node86::Node<Context>::impl(trait::Trait86::get) const
{
    return i + getNode(trait::trait85).get();
}

} // namespace di::bench::compile
