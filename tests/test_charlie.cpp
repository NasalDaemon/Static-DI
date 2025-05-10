#include <doctest/doctest.h>

#if !DI_IMPORT_STD
#include <concepts>
#include <cstdio>
#include <memory>
#include <utility>
#endif

import abc.charlie;
import abc.traits;
import di;
#if DI_IMPORT_STD
import std;
#endif

using namespace abc;

struct AliceMockStatic
{
    template<class Context>
    struct Node : di::Node
    {
        struct Types
        {
            using AliceType = int;
        };

        using Traits = di::Traits<Node, trait::Alice>;

        int apply(trait::Alice::get) const { return getNode(di::test::local(trait::charlie)).get(); }
    };
};

struct IAliceMock : di::INode
{
    struct Types
    {
        using AliceType = int;
    };
    using Traits = di::Traits<IAliceMock, trait::Alice>;

    virtual int apply(trait::Alice::get) const = 0;
};

struct AliceMockVirtualRemotes final : IAliceMock
{
    int apply(trait::Alice::get) const
    {
        return getNode(trait::charlie).get();
    }

    struct Remotes : di::IRemotes
    {
        struct Types
        {
            using CharlieType = int;
        };
        using Traits = di::Traits<Remotes, trait::Charlie>;

        virtual int apply(trait::Charlie::get) const = 0;
    };

    template<class Node>
    struct RemotesImpl final : di::IRemotesImpl<Node, Remotes>
    {
        using RemotesImpl::Base::Base;

        int apply(trait::Charlie::get) const
        {
            return this->getNode(trait::charlie).get();
        }
    };
};

template<class Node>
struct AliceMockStaticRemotes final : di::INodeImpl<Node, IAliceMock>
{
    using AliceMockStaticRemotes::Base::Base;

    int apply(trait::Alice::get) const
    {
        return this->getNode(di::test::local(trait::charlie)).get();
    }
};

TEST_CASE("di::test::Graph")
{
    using StaticCharlieTest = di::test::Graph<Charlie, AliceMockStatic>;
    StaticCharlieTest nonv;
    CHECK(109 == nonv.asTrait(trait::aliceRead).get());

    using VirtualCharlieTest = di::test::Graph<Charlie, di::Virtual<IAliceMock*>>;
    AliceMockVirtualRemotes mock;
    VirtualCharlieTest v1{.mocks{&mock}};
    CHECK(109 == v1.asTrait(trait::aliceRead).get());

    auto v2 = std::move(v1);
    CHECK(109 == v2.asTrait(trait::aliceRead).get());
}

TEST_CASE("di::INodeFactory")
{
    using G = di::test::Graph<Charlie, di::Virtual<std::unique_ptr<IAliceMock>>>;

    G g1{.mocks{di::INodeFactory<AliceMockStaticRemotes>{}}};
    CHECK(109 == g1.asTrait(trait::aliceRead).get());
    auto g2 = std::move(g1);
    CHECK(109 == g2.asTrait(trait::aliceRead).get());
}
