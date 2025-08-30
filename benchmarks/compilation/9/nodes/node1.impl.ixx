module di.bench.compile9.node1:impl;

import di.bench.compile9.node1;
import di;

namespace di::bench::compile9 {

template<class Context>
int Node1::Node<Context>::impl(trait::Trait1::get) const
{
    return i;
}

}
