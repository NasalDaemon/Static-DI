#pragma once

#include "abc/traits.hpp"

#include "di/node.hpp"
#include "di/resolve.hpp"
#include "di/traits.hpp"

namespace abc {

struct Charlie
{
    template<class Context>
    struct Node : di::Node
    {
        struct Alice;
        struct Charlie;
        struct Charlie2;
        struct Charlie3;

        struct AliceTypes
        {
            using AliceType = di::ResolveTypes<Context, trait::AliceRead>::AliceType;
        };
        struct CharlieTypes
        {
            using CharlieType = int;
        };

        using Traits = di::Traits<Node
            , trait::AliceRead(Alice, AliceTypes)
            , trait::Charlie(Charlie, CharlieTypes)
            , trait::Charlie2(Charlie2, CharlieTypes)
            , trait::Charlie3(Charlie3, CharlieTypes)
        >;

        using AliceType = AliceTypes::AliceType;
        using CharlieType = CharlieTypes::CharlieType;

    protected:
        AliceType charlie = 99;
    };
};

template<class Context>
struct Charlie::Node<Context>::Alice : Node
{
    int apply(trait::Alice::get) const;
};

template<class Context>
struct Charlie::Node<Context>::Charlie : Node
{
    int apply(trait::Charlie::get) const;
};

template<class Context>
struct Charlie::Node<Context>::Charlie2 : Node
{
    int apply(trait::Charlie::get) const
    {
        return -asTrait(trait::charlie).get();
    }
};

template<class Context>
struct Charlie::Node<Context>::Charlie3 : Node
{
    static int apply(trait::Charlie::get)
    {
        return 15;
    }
};

}
