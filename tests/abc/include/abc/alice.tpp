#pragma once

#include "abc/alice.hpp"

#define ALICE \
    template<class Context> \
    auto abc::Alice::Node<Context>

ALICE::apply(trait::Charlie::get) const -> int
{
    return getNode(trait::charlie).get();
}

ALICE::Alice::apply(trait::Alice::get) const -> int
{
    return alice;
}

ALICE::Alice::apply(trait::Alice::set, int value) -> void
{
    alice = value;
}

ALICE::Bob::apply(trait::Bob::get) const -> int
{
    return getNode(trait::bob).get();
}
