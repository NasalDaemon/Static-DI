#include <doctest/doctest.h>

#if !DI_STD_MODULE
#include <cstdio>
#endif

import abc.charlie;
import abc.traits;
import di;
DI_IMPORT_STD;

using namespace abc;

struct ICharlieMocks : di::INode
{
    struct Types
    {
        using AliceType = int;
    };
    using Traits = di::TraitsOpen<ICharlieMocks>;

    virtual int apply(trait::Alice::get) const = 0;

    struct Remotes : di::IRemotes
    {
        struct Types
        {
            using CharlieType = int;
        };
        using Traits = di::TraitsOpen<Remotes>;

        virtual int apply(trait::Charlie::get) const = 0;
    };

    template<class VirtualNode>
    struct RemotesImpl final : Remotes
    {
        explicit RemotesImpl(di::Alias<VirtualNode> node) : node(node) {}
        di::Alias<VirtualNode> node;

        int apply(trait::Charlie::get) const override
        {
            return node->getNode(trait::charlie).get();
        }
    };
};

using VCharlieTest = di::test::Graph<Charlie, di::Virtual<ICharlieMocks*>>;

struct SCharlieMocks {
    template<class Context>
    struct Node : di::Node
    {
        struct Types
        {
            using AliceType = int;
        };

        using Traits = di::TraitsOpen<Node>;

        int apply(trait::Alice::get) const { return getNode(di::test::local(trait::charlie)).get(); }
    };
};

using SCharlieTest = di::test::Graph<Charlie, SCharlieMocks>;

TEST_CASE("di::test::Graph")
{
    struct Mock final : ICharlieMocks
    {
        int apply(trait::Alice::get) const override
        {
            return getNode(trait::charlie).get();
        }
    } mock;

    VCharlieTest virt{.mocks{&mock}};
    CHECK(109 == virt.asTrait(trait::aliceRead).get());

    SCharlieTest nonv;
    CHECK(109 == nonv.asTrait(trait::aliceRead).get());
}
