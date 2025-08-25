module di.bench.compile.node93:impl;
import di.bench.compile.node93;

namespace di::bench::compile {

template<class Context>
int Node93::Node<Context>::impl(trait::Trait93::get) const
{
    return i + getNode(trait::trait92).get();
}

} // namespace di::bench::compile
