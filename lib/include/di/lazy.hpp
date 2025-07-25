#pragma once

#include "di/detail/cast.hpp"
#include "di/detail/compress.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/trait.hpp"

#if !DI_IMPORT_STD
#include <memory>
#include <functional>
#include <utility>
#include <variant>
#endif

namespace di {

DI_MODULE_EXPORT
template<IsNodeHandle Underlying>
struct Lazy
{
    template<class Context>
    struct Node : di::Node
    {
    private:
        [[using DI_IF_GNU_ELSE(gnu)(msvc): noinline, cold]]
        void initialise() const
        {
            auto* init = std::get_if<InitialiserPtr>(&state);
            [[assume(init != nullptr)]];
            auto initKeepAlive = std::move(*init);
            (*initKeepAlive)(this);
        }

        struct InnerContext : Context
        {
            template<IsTrait Trait>
            requires detail::HasLink<Context, Trait>
            static constexpr auto getNode(auto& state, Trait trait)
            {
                // Assumption that variant-to-element offset=0 is tested in test_lazy.cpp
                auto const nodePtr = std::bit_cast<Node NodeState::*>(-detail::memPtrToInt(&Node::state));
                return Context{}.getNode(detail::downCast<NodeState>(state).*nodePtr, trait);
            }
        };

        using NodeState = ToNodeWrapper<Underlying>::template Node<detail::CompressContext<InnerContext>>;
        using Initialiser = std::function<void(Node const*)>;
        using InitialiserPtr = std::unique_ptr<Initialiser>;
        using Variant = std::variant<NodeState, InitialiserPtr>;
        Variant mutable state;

        template<class Trait>
        requires HasTrait<NodeState, Trait>
        using TraitsTemplate = di::ResolvedTrait<Node, typename detail::ResolveTrait<NodeState, Trait>::type::Types>;

    public:
        using Traits = di::TraitsTemplate<Node, TraitsTemplate>;

        DI_INLINE constexpr decltype(auto) impl(this auto& self, auto&&... args)
        {
            return self.getState().impl(DI_FWD(args)...);
        }

        template<class Self>
        constexpr auto& getState(this Self& self)
        {
            if (not std::holds_alternative<NodeState>(self.state)) [[unlikely]]
                self.initialise();
            auto* node = std::get_if<NodeState>(&self.state);
            [[assume(node != nullptr)]];
            return std::forward_like<Self&>(*node);
        }

        constexpr explicit Node(auto&&... args)
            : state(std::make_unique<Initialiser>(
                [...args = DI_FWD(args)](Node const* self) mutable -> void
                {
                    self->state.template emplace<NodeState>(std::move(args)...);
                }))
        {}

        Node(Node&&) = default;
        constexpr Node(Node const& other)
            : state(
                [&]() -> Variant
                {
                    if (InitialiserPtr const* init = std::get_if<InitialiserPtr>(&other.state))
                        return Variant(std::make_unique<Initialiser>(**init));
                    else
                        return Variant(std::get<NodeState>(other.state));
                }())
        {}
    };
};

}
