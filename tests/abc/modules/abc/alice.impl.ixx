export module abc.alice:impl;

import abc.alice;
import abc.traits;

#define ALICE \
    template<class Context> \
    auto abc::Alice::Node<Context>

ALICE::apply(trait::Bob::get) const -> int
{
    return getNode(trait::bob).get();
}
ALICE::apply(trait::Bob::set, int) -> void
{
}

ALICE::apply(trait::Charlie::get method) const -> int
{
    return getNode(traitOf(method)).get();
}
