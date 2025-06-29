#pragma once

#include "abc/traits.hpp"

#include "di/node.hpp"
#include "di/requires.hpp"
#include "di/resolve.hpp"
#include "di/traits.hpp"

namespace abc {

struct Bob
{
    template<class Context>
    struct Node : di::Node
    {
        using Requires = di::Requires<trait::Alice, trait::Charlie>;

        using Traits = di::Traits<Node
            , trait::AliceRead
            , trait::Bob
            , trait::Charlie
        >;

        int apply(trait::Alice::get) const;

        int apply(trait::Bob::get) const;
        void apply(trait::Bob::set, int value);

        int apply(trait::Charlie::get) const;

        struct Types
        {
            using BobType = int;
            using CharlieType = di::ResolveTypes<Node, trait::Charlie>::CharlieType;
        };

        using AliceType = di::ResolveTypes<Node, trait::Alice>::AliceType;
        using BobType = Types::BobType;
        using CharlieType = Types::CharlieType;

    private:
        AliceType bob = 64;
    };
};

}
