module di.bench.compile9.node5:impl;

import di.bench.compile9.node5;

namespace di::bench::compile9 {

template<class Context>
int Node5::Node<Context>::impl(trait::Trait5::get) const
{
    return i + getNode(trait::trait4).get();
}

}
