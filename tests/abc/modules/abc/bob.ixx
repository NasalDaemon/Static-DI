module;
#if !DI_IMPORT_STD
#include <cstdio>
#endif
export module abc.bob;

import abc.traits;
import di;
#if DI_IMPORT_STD
import std;
#endif

export namespace abc {

struct Bob
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Alice, trait::Charlie>;

        using Traits = di::Traits<Node
            , trait::AliceRead
            , trait::Bob
            , trait::Charlie
        >;

        void onGraphConstructed() { std::puts("Constructed Bob"); }

        int apply(trait::Alice::get) const
        {
            return getNode(trait::alice).get();
        }

        int apply(trait::Bob::get) const { return bob; }
        void apply(trait::Bob::set, int value) { bob = value; }

        int apply(trait::Charlie::get) const
        {
            return getNode(trait::charlie).get();
        }

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
