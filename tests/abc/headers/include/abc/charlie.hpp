#pragma once

#include "abc/traits.hpp"

#include "di/depends.hpp"
#include "di/node.hpp"
#include "di/resolve.hpp"
#include "di/traits.hpp"

namespace abc {

struct Charlie
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Alice>;

        struct Alice;
        struct Charlie;
        struct Charlie2;
        struct Charlie3;

        struct AliceTypes;
        struct CharlieTypes;

        using Traits = di::Traits<Node
            , trait::AliceRead(Alice, AliceTypes)
            , trait::Charlie(Charlie, CharlieTypes)
            , trait::Charlie2(Charlie2, CharlieTypes)
            , trait::Charlie3(Charlie3, CharlieTypes)
            , trait::Visitable
        >;

        struct AliceTypes
        {
            using AliceType = di::ResolveTypes<Node, trait::AliceRead>::AliceType;
        };
        struct CharlieTypes
        {
            using CharlieType = int;
        };

        using AliceType = AliceTypes::AliceType;
        using CharlieType = CharlieTypes::CharlieType;

        void apply(trait::Visitable::count, int& counter);

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
