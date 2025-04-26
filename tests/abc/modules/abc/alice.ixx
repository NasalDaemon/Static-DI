module;
#if !DI_STD_MODULE
#include <type_traits>
#endif
export module abc.alice;

import abc.traits;
import di;
#if DI_STD_MODULE
import std;
#endif

export namespace abc {

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
        struct Types
        {
            using AliceType = int;
            using BobType = di::ResolveTypes<Context, trait::Bob>::BobType;
            using CharlieType = di::ResolveTypes<Context, trait::Bob>::CharlieType;
        };

        using Traits = di::Traits<Node
            , trait::Alice*(Types)
            , trait::Bob
            , trait::Charlie
            , trait::Visitable
        >;

        using AliceType = Types::AliceType;
        using BobType = Types::BobType;

        using NodeBase::apply;

        int apply(trait::Bob::get) const;
        void apply(trait::Bob::set, int);

        int apply(trait::Charlie::get method) const;

    private:
        static_assert(std::is_same_v<decltype(NodeBase::alice), BobType>);
        static_assert(std::is_same_v<di::NullContext::Root, di::ResolveRoot<Context>>);
        static_assert(std::is_same_v<di::NullContext::Info, di::ResolveInfo<Context>>);
    };
};

}
