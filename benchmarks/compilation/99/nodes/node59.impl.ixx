module di.bench.compile.node59:impl;
import di.bench.compile.node59;

namespace di::bench::compile {

template<class Context>
int Node59::Node<Context>::impl(trait::Trait59::get) const
{
    return i + getNode(trait::trait58).get();
}

} // namespace di::bench::compile
