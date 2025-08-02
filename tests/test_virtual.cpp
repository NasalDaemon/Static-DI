#include <doctest/doctest.h>
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <memory>
#include <type_traits>
#include <utility>
#endif

import di.tests.virtual_;
import di;


/*
di-embed-begin

export module di.tests.virtual_;

namespace di::tests::virtual_ {

trait trait::Apple
{
    seeds() const -> int
    testExchange() -> bool
}

trait trait::Bread
{
    slices() const -> int
}

trait trait::Egg
{
    yolks() const -> int
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
    virtual int impl(trait::Apple::seeds) const = 0;
    virtual bool impl(trait::Apple::testExchange) = 0;
};
struct IBread : di::INode
{
    using Traits = di::Traits<IBread, trait::Bread>;
    struct Types
    {
        using BreadType = virtual_::BreadType;
    };
    virtual int impl(trait::Bread::slices) const = 0;
};
struct IEgg : di::INode
{
    using Traits = di::Traits<IEgg, trait::Egg>;
    virtual int impl(trait::Egg::yolks) const = 0;
};

struct AppleEgg
{
    template<class Context>
    struct Node : IApple, IEgg
    {
        using Traits = di::Traits<Node, trait::Apple, trait::Egg>;

        static_assert(std::is_same_v<BreadType, typename di::ResolveTypes<Node, trait::Bread>::BreadType>);

        int impl(trait::Apple::seeds) const final
        {
            return seeds + getNode(trait::bread).slices();
        }

        int impl(trait::Egg::yolks) const final
        {
            return yolks;
        }

        bool impl(trait::Apple::testExchange) final
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
                return true;
            }
            return false;
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

        static_assert(std::is_same_v<AppleType, typename di::ResolveTypes<Node, trait::Apple>::AppleType>);

        int impl(trait::Bread::slices) const final
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

    CHECK(g2.asTrait(trait::apple).testExchange());
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
        int impl(trait::Egg::yolks) const
        {
            return yolks;
        }
        int impl(trait::Bread::slices) const
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

struct StaticBread
{
    template<class Context>
    struct Node : di::Node
    {
        using Traits = di::Traits<Node, trait::Bread>;

        int impl(trait::Bread::slices) const
        {
            return slices + getNode(trait::egg).yolks();
        }

        explicit Node(int slices = 42) : slices(slices) {}

        int slices;
    };
};

struct BreadFacade
{
    template<class Context>
    struct Node final : IApple, IBread
    {
        using Traits = di::Traits<Node
            , trait::Apple*(IApple::Types)
            , trait::Bread*(IBread::Types)
        >;

        int impl(trait::Apple::seeds) const { return 0; }
        bool impl(trait::Apple::testExchange)
        {
            if constexpr (di::IsVirtualContext<Context>)
            {
                auto handle = exchangeImpl<di::Adapt<StaticBread, BreadFacade>>();
                int expected = 99 + 42 + 1;
                CHECK(asTrait(trait::bread).slices() == expected);
                CHECK(handle.getNext().asTrait(trait::bread).slices() == expected);
                return true;
            }
            return false;
        }

        int impl(trait::Bread::slices) const
        {
            return test + getNode(trait::bread).slices();
        }

        explicit Node(int test = 99) : test(test) {}

        int test;
    };
};

struct EggDouble
{
    template<class Context>
    struct Node : di::Node
    {
        using Traits = di::Traits<Node, trait::Egg>;

        int impl(trait::Egg::yolks) const { return yolks; }

        explicit Node(int yolks = 1) : yolks(yolks) {}
        int yolks;
    };
};

TEST_CASE("di::Virtual with di::Adapt")
{
    di::test::Graph<di::Virtual<IApple, IBread>, EggDouble> g{
        .node{di::adapt<StaticBread, BreadFacade>}
    };

    int expected = 99 + 42 + 1;
    CHECK(g.asTrait(trait::bread).slices() == expected);
    CHECK(g.asTrait(trait::apple).testExchange());
}

struct EggFacade
{
    template<class Context>
    struct Node final : IEgg
    {
        using Traits = di::Traits<Node, trait::Egg>;

        int impl(trait::Egg::yolks) const
        {
            return getNode(trait::egg).yolks();
        }
    };
};

/*
di-embed-begin

cluster EggBread [R = Root]
{
    egg = R::StaticEgg
    bread = R::VirtualBread

    [trait::Egg <-> trait::Bread]
    egg <-> bread
}

di-embed-end
*/

DI_INSTANTIATE_BOX(StaticBread, BreadFacade, EggFacade, IEgg)

TEST_CASE("di::Box")
{
    struct Root
    {
        using StaticEgg = virtual_::EggDouble;
        using VirtualBread = di::Virtual<IBread>;
    };

    di::Graph<EggBread, Root> g{
        .bread{di::box<StaticBread, BreadFacade, EggFacade, IEgg>, di::args<StaticBread>(41), di::args<BreadFacade>(98)}
    };

    CHECK(g.bread.asTrait(trait::bread).slices() == 140);

    auto g2 = std::move(g);
    CHECK(g2.bread.asTrait(trait::bread).slices() == 140);
}

DI_INSTANTIATE_BOX(EggDouble, EggFacade)

TEST_CASE("di::Box with no outnode")
{
    di::test::Graph<di::Virtual<IEgg>> g{
        .node{di::box<EggDouble, EggFacade>, 4}
    };

    CHECK(g.node.asTrait(trait::egg).yolks() == 4);

    auto g2 = std::move(g);
    CHECK(g2.node.asTrait(trait::egg).yolks() == 4);
}

// di-embed-begin

} // namespace di::tests::virtual_

// di-embed-end
