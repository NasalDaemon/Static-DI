#pragma once

#include "abc/bob.hpp"

#define BOB \
    template<class Context> \
    auto abc::Bob::Node<Context>

BOB::apply(trait::Alice::get) const -> int
{
    return getNode(trait::alice).get();
}

BOB::apply(trait::Bob::get) const -> int
{
    return bob;
}

BOB::apply(trait::Bob::set, int value) -> void
{
    bob = value;
}

BOB::apply(trait::Charlie::get) const -> int
{
    return getNode(trait::charlie).get();
}
