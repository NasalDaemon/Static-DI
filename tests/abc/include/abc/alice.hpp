#pragma once

#include "abc/traits.hpp"

#include "di/context.hpp"
#include "di/node.hpp"
#include "di/resolve.hpp"
#include "di/traits.hpp"

namespace abc {

struct Alice
{
    template<class Context>
    struct Node : di::Node
    {
        struct Alice;
        struct Bob;

        using Traits = di::Traits<Node
            , trait::Alice(Alice)
            // , trait::AliceRead(Alice)
            , trait::Bob(Bob)
            , trait::Charlie
        >;

        struct Types
        {
            using AliceType = int;
        };

        using AliceType = Types::AliceType;
        using BobType = di::ResolveTypes<Context, trait::Bob>::BobType;
        using CharlieType = di::ResolveTypes<Context, trait::Bob>::CharlieType;

        int apply(trait::Charlie::get) const;

    protected:
        BobType alice = 92;

        static_assert(std::is_same_v<di::NullContext::Root, di::ResolveRoot<Context>>);
        static_assert(std::is_same_v<di::NullContext::Info, di::ResolveInfo<Context>>);
    };
};

template<class Context>
struct Alice::Node<Context>::Alice : Node
{
    int apply(trait::Alice::get) const;
    void apply(trait::Alice::set, int value);
};

template<class Context>
struct Alice::Node<Context>::Bob : Node
{
    int apply(trait::Bob::get) const;
};

}
