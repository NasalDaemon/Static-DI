#pragma once

#include "abc/charlie.hpp"

#include "di/test.hpp"

#define CHARLIE \
    template<class Context> \
    auto abc::Charlie::Node<Context>

CHARLIE::Alice::apply(trait::Alice::get) const -> int
{
    static_assert(di::CanGetNode<Node, trait::AliceRead>);
    auto const value = getNode(trait::aliceRead).get();

    if constexpr (di::test::IsTestContext<Context>)
        return value + 10;
    else
        return value;
};

CHARLIE::Charlie::apply(trait::Charlie::get) const -> int
{
     return charlie;
}
