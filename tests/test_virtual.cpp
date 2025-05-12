#include <doctest/doctest.h>
#include <utility>
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <memory>
#include <type_traits>
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

struct IApple : virtual di::INode
{
    using Traits = di::Traits<IApple, trait::Apple>;
    struct Types
    {
        using AppleType = virtual_::AppleType;
    };
    virtual int apply(trait::Apple::seeds) const = 0;
    virtual void apply(trait::Apple::testExchange) = 0;
};
struct IBread : virtual di::INode
{
    using Traits = di::Traits<IBread, trait::Bread>;
    struct Types
    {
        using BreadType = virtual_::BreadType;
    };
    virtual int apply(trait::Bread::slices) const = 0;
};
struct IEgg : virtual di::INode
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

        int apply(trait::Apple::seeds) const
        {
            return seeds + getNode(trait::bread).slices();
        }

        int apply(trait::Egg::yolks) const
        {
            return yolks;
        }

        void apply(trait::Apple::testExchange)
        {
            CHECK(asTrait(trait::apple).seeds() == 34);
            auto [current, next] = exchangeImplementation(std::in_place_type<AppleEgg>, 11);
            REQUIRE(current.get() == this);
            REQUIRE(std::addressof(next) != this);
            CHECK(next.asTrait(trait::apple).seeds() == 35);
        }

        explicit Node(int seeds = 10, int yolks = 2)
            : seeds(seeds), yolks(yolks)
        {}

        int seeds;
        int yolks;
    };
};

struct Bread
{
    template<class Context>
    struct Node : IBread
    {
        using Traits = di::Traits<Node, trait::Bread>;

        static_assert(std::is_same_v<AppleType, typename di::ResolveTypes<Context, trait::Apple>::AppleType>);

        int apply(trait::Bread::slices) const
        {
            return getNode(trait::egg).yolks() * slices;
        }

        void onGraphConstructed()
        {
            CHECK(slices == 314);
            slices = 12;
        }

        int slices = 314;
    };
};

TEST_CASE("di::Virtual")
{
    using G = di::test::Graph<di::Virtual<IApple, IEgg>, di::Virtual<IBread>>;

    G g{
        .node{std::in_place_type<AppleEgg>},
        .mocks{std::in_place_type<Bread>}
    };
    g.onConstructed();

    CHECK(g.asTrait(trait::apple).seeds() == 34);
    CHECK(g.asTrait(trait::egg).yolks() == 2);
    CHECK(g.mocks.asTrait(trait::bread).slices() == 24);

    auto g2 = std::move(g);
    CHECK(g2.asTrait(trait::apple).seeds() == 34);
    CHECK(g2.asTrait(trait::egg).yolks() == 2);
    CHECK(g2.mocks.asTrait(trait::bread).slices() == 24);

    g2.asTrait(trait::apple).testExchange();
}

}
