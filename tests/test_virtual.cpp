#include <doctest/doctest.h>
#include <utility>
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <memory>
#include <type_traits>
#include <utility>
#endif

import di.tests.virtual_;
import di;
#if DI_IMPORT_STD
import std;
#endif


/*
di-embed-begin

export module di.tests.virtual_;

namespace di::tests::virtual_ {

trait trait::Apple
{
    seeds() const -> int
    testExchange()
}

trait trait::Bread
{
    slices() const -> int
}

trait trait::Egg
{
    yolks() const -> int
}

}

di-embed-end
*/

namespace di::tests::virtual_ {

struct AppleType;
struct BreadType;

struct IApple : di::INode
{
    using Traits = di::Traits<IApple, trait::Apple>;
    struct Types
    {
        using AppleType = virtual_::AppleType;
    };
    virtual int apply(trait::Apple::seeds) const = 0;
    virtual void apply(trait::Apple::testExchange) = 0;
};
struct IBread : di::INode
{
    using Traits = di::Traits<IBread, trait::Bread>;
    struct Types
    {
        using BreadType = virtual_::BreadType;
    };
    virtual int apply(trait::Bread::slices) const = 0;
};
struct IEgg : di::INode
{
    using Traits = di::Traits<IEgg, trait::Egg>;
    virtual int apply(trait::Egg::yolks) const = 0;
};

struct AppleEgg
{
    template<class Context>
    struct Node : IApple, IEgg
    {
        using Traits = di::Traits<Node, trait::Apple, trait::Egg>;

        static_assert(std::is_same_v<BreadType, typename di::ResolveTypes<Context, trait::Bread>::BreadType>);

        int apply(trait::Apple::seeds) const final
        {
            return seeds + getNode(trait::bread).slices();
        }

        int apply(trait::Egg::yolks) const final
        {
            return yolks;
        }

        void apply(trait::Apple::testExchange) final
        {
            if constexpr (di::IsVirtualContext<Context>)
            {
                di::KeepAlive keepAlive;
                {
                    CHECK(asTrait(trait::apple).seeds() == 34);
                    auto handle = exchangeImpl<AppleEgg>(11, 3);
                    allowDestroy = false;
                    REQUIRE(std::addressof(handle.getNext()) != this);
                    CHECK(handle.getNext().asTrait(trait::apple).seeds() == 47);
                    // Using current node still works, but "re-entry" to this context from apple
                    // will go to the new node that was just emplaced into the graph instead
                    CHECK(asTrait(trait::apple).seeds() == 46);
                    keepAlive = handle;
                }
                CHECK(asTrait(trait::apple).seeds() == 46);
                allowDestroy = true;
            }
        }

        explicit Node(int seeds = 10, int yolks = 2)
            : seeds(seeds), yolks(yolks)
        {}

        ~Node()
        {
            REQUIRE(allowDestroy);
        }

        int seeds;
        int yolks;
        bool allowDestroy = true;
    };
};

struct Bread
{
    template<class Context>
    struct Node : IBread
    {
        using Traits = di::Traits<Node, trait::Bread>;

        // Prove that withFactory works with copy/move elision
        Node() = default;
        Node(Node const&) = delete;
        Node(Node&&) = delete;

        static_assert(std::is_same_v<AppleType, typename di::ResolveTypes<Context, trait::Apple>::AppleType>);

        int apply(trait::Bread::slices) const final
        {
            return getNode(trait::egg).yolks() * slices;
        }

        void onGraphConstructed() final
        {
            CHECK(slices == 314);
            slices = 12;
        }

        int slices = 314;
    };
};

void test(auto& g)
{
    g.onConstructed();

    CHECK(g.asTrait(trait::apple).seeds() == 34);
    CHECK(g.asTrait(trait::egg).yolks() == 2);
    CHECK(g.mocks.asTrait(trait::bread).slices() == 24);
}

void testExchange(auto& g)
{
    auto g2 = std::move(g);
    CHECK(g2.asTrait(trait::apple).seeds() == 34);
    CHECK(g2.asTrait(trait::egg).yolks() == 2);
    CHECK(g2.mocks.asTrait(trait::bread).slices() == 24);

    g2.asTrait(trait::apple).testExchange();
}

TEST_CASE("di::Virtual")
{
    using Virtual = di::test::Graph<di::Virtual<IApple, IEgg>, di::Virtual<IBread>>;

    Virtual virt{
        .node{std::in_place_type<AppleEgg>},
        .mocks{di::withFactory, []<class C>(C) -> C::type { return std::in_place_type<Bread>; }}
    };
    test(virt);
    testExchange(virt);

    using Static = di::test::Graph<AppleEgg, Bread>;
    Static stat;
    test(stat);
}

struct VirtualOnly
{
    struct Leaf final : IEgg, IBread
    {
        int apply(trait::Egg::yolks) const
        {
            return yolks;
        }
        int apply(trait::Bread::slices) const
        {
            return slices;
        }

        explicit Leaf(int yolks = 999, int slices = 88)
            : yolks(yolks), slices(slices)
        {}

        int yolks;
        int slices;
    };

    template<class Context>
    using Node = Leaf;
};

TEST_CASE("di::Virtual: Virtual-only node may be final without traits and simple leaf")
{
    di::test::Graph<di::Virtual<IBread, IEgg>> g{.node{std::in_place_type<VirtualOnly::Leaf>}};

    CHECK(g.asTrait(trait::egg).yolks() == 999);
    CHECK(g.asTrait(trait::bread).slices() == 88);

    g.node->emplace<VirtualOnly>(11, 333);

    CHECK(g.asTrait(trait::egg).yolks() == 11);
    CHECK(g.asTrait(trait::bread).slices() == 333);
}

}
