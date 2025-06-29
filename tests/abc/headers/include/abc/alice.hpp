#pragma once

#include "abc/traits.hpp"

#include "di/context.hpp"
#include "di/node.hpp"
#include "di/requires.hpp"
#include "di/resolve.hpp"
#include "di/traits.hpp"

namespace abc {

struct Alice
{
    struct NodeBase : di::Node
    {
        void onGraphConstructed();

        int apply(trait::Alice::get) const;
        void apply(trait::Alice::set, int value);

        void apply(trait::Visitable::count, int& counter);

        int alice = 92;
    };

    template<class Context>
    struct Node : NodeBase
    {
        using Requires = di::Requires<trait::Bob, trait::Charlie>;

        struct Types;

        using Traits = di::Traits<Node
            , trait::Alice*(Types)
            , trait::Bob
            , trait::Charlie
            , trait::Visitable
        >;

        struct Types
        {
            using AliceType = int;
            using BobType = di::ResolveTypes<Node, trait::Bob>::BobType;
            using CharlieType = di::ResolveTypes<Node, trait::Bob>::CharlieType;
        };

        using AliceType = Types::AliceType;
        using BobType = Types::BobType;

        using NodeBase::apply;

        int apply(trait::Bob::get) const;
        void apply(trait::Bob::set, int);

        int apply(trait::Charlie::get method) const;

        static_assert(std::is_same_v<decltype(NodeBase::alice), BobType>);
        static_assert(std::is_same_v<di::NullContext::Root, di::ResolveRoot<Context>>);
        static_assert(std::is_same_v<di::NullContext::Info, di::ResolveInfo<Context>>);
    };
};

}
