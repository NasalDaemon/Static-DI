#ifndef INCLUDE_DI_VIRTUAL_HPP
#define INCLUDE_DI_VIRTUAL_HPP

#include "di/detail/cast.hpp"
#include "di/alias.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/traits.hpp"
#include "di/empty_types.hpp"

#if !DI_STD_MODULE
#include <memory>
#include <type_traits>
#include <utility>
#endif

namespace di {

DI_MODULE_EXPORT
struct IRemotes
{
    using Types = EmptyTypes;
    using Traits = di::TraitsOpen<IRemotes>;

    virtual ~IRemotes() = default;

    template<class Self>
    constexpr auto asTrait(this Self& self, auto trait, auto... keys)
    {
        return TraitView(trait, Alias(self, keys...), std::type_identity<typename Self::Types>{});
    }
};

DI_MODULE_EXPORT
struct INode
{
    using Types = EmptyTypes;
    using Traits = di::TraitsOpen<INode>;

    virtual ~INode() = default;

    using Remotes = void;

    template<class Interface, class Virtual>
    constexpr void injectRemotes(this Interface& self, Virtual& virtualNode)
    {
        using Remotes = Interface::Remotes;
        if constexpr (not std::is_void_v<Remotes>)
        {
            static_assert(std::derived_from<Remotes, IRemotes>);
            using RemotesImpl = Interface::template RemotesImpl<Virtual>;
            static_assert(std::derived_from<RemotesImpl, Remotes>);
            static_assert(std::is_final_v<RemotesImpl>);
            self.remotes = std::make_unique<RemotesImpl>(di::Alias(virtualNode));
        }
    }

protected:
    template<class Self>
    constexpr auto getNode(this Self& self, auto trait)
    {
        using Remotes = Self::Remotes;
        static_assert(not std::is_void_v<Remotes>, "Remotes struct not defined in Interface");
        auto& remotes = detail::downCast<Remotes>(*self.remotes);
        return std::forward_like<Self&>(remotes).asTrait(trait);
    }

private:
    std::unique_ptr<IRemotes> remotes;
};

DI_MODULE_EXPORT
template<class InterfacePtr, class... TraitTs>
struct Virtual
{
    template<class Context>
    struct Node : di::Node
    {
        explicit constexpr Node(InterfacePtr p) : impl(std::move(p))
        {
            impl->injectRemotes(*this);
        }

        Node(Node const&) = delete;
        Node(Node&& other) : impl(std::move(other.impl))
        {
            impl->injectRemotes(*this);
        }

        // Allow interface implementation to be changed dynamically
        constexpr void setImplementation(InterfacePtr p)
        {
            impl = std::move(p);
            impl->injectRemotes(*this);
        }

        using Interface = std::remove_cvref_t<decltype(*std::declval<InterfacePtr&>())>;
        static_assert(std::is_base_of_v<INode, Interface>);
        using Types = Interface::Types;

        template<class Self>
        constexpr decltype(auto) apply(this Self& self, auto&&... args)
        {
            return std::forward_like<Self&>(*self.impl).apply(DI_FWD(args)...);
        }

        using Traits = std::conditional_t<sizeof...(TraitTs) == 0,
            di::TraitsOpen<Node>,
            di::Traits<Node, TraitTs...>
        >;

    private:
        InterfacePtr impl;
    };
};

} // namespace di


#endif // INCLUDE_DI_VIRTUAL_HPP
