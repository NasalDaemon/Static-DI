module;
#if !DI_IMPORT_STD
#include <cstdio>
#include <type_traits>
#endif
export module abc.charlie;

import abc.traits;
import di;
#if DI_IMPORT_STD
import std;
#endif

export namespace abc {

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

        struct Charlie : di::DetachedInterface
        {
            template<class Self>
            int apply(this Self const& self, trait::Charlie::get)
            {
                using AliceType = di::ResolveTypes<Self, trait::Alice>::AliceType;
                static_assert(std::is_same_v<int, AliceType>);
                return self->charlie;
            }
        };

        struct Charlie2 : di::DetachedInterface
        {
            int apply(this auto const& self, trait::Charlie::get)
            {
                return -self.asTrait(trait::charlie).get();
            }
        };

        struct Charlie3 : di::DetachedInterface
        {
            static int apply(trait::Charlie::get)
            {
                return 15;
            }
        };

        using AliceType = AliceTypes::AliceType;
        using CharlieType = CharlieTypes::CharlieType;

        void onGraphConstructed() { std::puts("Constructed Charlie"); }

        void apply(trait::Visitable::count, int& counter)
        {
            std::puts("trait::Visitable::count: Visited Charlie");
            counter++;
        }

        ~Node() { std::puts("~Charlie"); }

    protected:
        AliceType charlie = 99;
    };
};

template<class Context>
struct Charlie::Node<Context>::Alice : Node
{
    int apply(trait::Alice::get) const
    {
        static_assert(di::CanGetNode<Node, trait::AliceRead>);
        auto const value = getNode(trait::aliceRead).get();

        if constexpr (di::test::IsTestContext<Context>)
            return value + 10;
        else
            return value;
    }
};

}
