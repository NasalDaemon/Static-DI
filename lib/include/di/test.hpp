#ifndef INCLUDE_DI_TEST_HPP
#define INCLUDE_DI_TEST_HPP

#include "di/cluster.hpp"
#include "di/context.hpp"
#include "di/global_graph.hpp"
#include "di/graph.hpp"
#include "di/key.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/map_info.hpp"
#include "di/mock_fwd.hpp"
#include "di/node.hpp"
#include "di/test_context.hpp"
#include "di/trait.hpp"

namespace di::test {

DI_MODULE_EXPORT
template<IsTrait Trait>
struct Local : Trait
{
    static TraitExpects<Trait> expects();
};

DI_MODULE_EXPORT
template<IsTrait Trait>
constexpr Local<Trait> local(Trait = {}) { return {}; }

DI_MODULE_EXPORT
template<IsTrait Trait>
struct MockTrait : Trait
{
    static TraitExpects<Trait> expects();

    // Mocks are to bypass checks, and may implement only what is needed for testing
    template<class Self, class...>
    using Implements = void;
};

DI_MODULE_EXPORT
struct MockKey : di::key::Default
{
    template<class T>
    using Trait = MockTrait<T>;
};

DI_MODULE_EXPORT
struct TestOnlyNode : di::Node
{
    template<class Self>
    static constexpr void assertNodeContext()
    {
        static_assert(IsTestContext<ContextOf<Self>>, "This node may only be used in a test context.");
        di::Node::assertNodeContext<Self>();
    }
};

namespace detail {

    struct TestMapInfo
    {
        template<IsContext Context>
        struct MapInfo : Context::Info
        {
            static void isTestContext(detail::TestContextTag);
        };
    };

    template<IsNodeHandle NodeT, IsNodeHandle MocksT>
    struct Cluster
    {
        template<class Context>
        struct Impl : di::Cluster
        {
            struct Node;
            struct Mocks;

            template<class Trait>
            static ResolvedLink<Node, Trait> resolveLink(Trait);

            struct Node : di::Context<Impl, NodeT>
            {
                static constexpr std::size_t Depth = Context::Depth;

                // Resolve to parent by default
                template<class Trait>
                requires di::detail::HasLink<Context, Trait>
                static ResolvedLink<Context, Trait> resolveLink(Trait);

                // Otherwise resolve to mocks
                template<class Trait>
                static ResolvedLink<Mocks, Trait> resolveLink(Trait);

                // getNode calls to mocks, so allow partial implementation of traits
                struct Info : Context::Info
                {
                    using DefaultKey = MockKey;
                };
            };

            struct Mocks : di::Context<Impl, MocksT>
            {
                static constexpr std::size_t Depth = Context::Depth;

                // Resolve to parent by default
                template<class Trait>
                requires di::detail::HasLink<Context, Trait>
                static ResolvedLink<Context, Trait> resolveLink(Trait);

                // Otherwise resolve to node being tested
                template<class Trait>
                static ResolvedLink<Node, Trait> resolveLink(Trait);

                // Allow explicitly resolving the node being tested
                template<class Trait>
                static ResolvedLink<Node, Trait> resolveLink(Local<Trait>);
            };

            DI_NODE(Node, node)
            DI_NODE(Mocks, mocks)

            constexpr void visit(this auto& self, auto&& visitor)
            {
                self.node.visit(visitor);
                self.mocks.visit(visitor);
            }
        };

        template<class Context>
        using Node = Impl<Context>;
    };

} // namespace detail

DI_MODULE_EXPORT
template<IsNodeHandle Node, IsNodeHandle Mocks = Mock<>>
using Cluster = MapInfo<detail::Cluster<Node, Mocks>, detail::TestMapInfo>;

DI_MODULE_EXPORT
template<IsNodeHandle Node, IsNodeHandle Mocks = Mock<>, class Root = void>
using Graph = di::Graph<Cluster<Node, Mocks>, Root>;

DI_MODULE_EXPORT
template<IsNodeHandle Node, class GlobalNode, IsNodeHandle Mocks = Mock<>, class Root = void>
using GraphWithGlobal = di::GraphWithGlobal<Cluster<Node, Mocks>, GlobalNode, Root>;

} // namespace di::test


#endif // INCLUDE_DI_TEST_HPP
