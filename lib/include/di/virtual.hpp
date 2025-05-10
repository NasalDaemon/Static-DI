#ifndef INCLUDE_DI_VIRTUAL_HPP
#define INCLUDE_DI_VIRTUAL_HPP

#include "di/detail/cast.hpp"
#include "di/alias.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/traits.hpp"
#include "di/empty_types.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>
#endif

namespace di {

DI_MODULE_EXPORT
struct INode;

DI_MODULE_EXPORT
template<class InterfacePtr, class... TraitTs>
struct Virtual
{
    template<class Context>
    struct Node : di::Node
    {
        template<class T>
        requires std::convertible_to<T&&, InterfacePtr>
        explicit constexpr Node(T&& p)
            : impl((p->injectRemotes(*this), std::forward<T>(p)))
        {}

        Node(Node const&) = delete;
        constexpr Node(Node&& other) : impl(std::exchange(other.impl, nullptr))
        {
            if (impl)
                impl->injectRemotes(*this);
        }

        // Allow interface implementation to be changed dynamically
        template<class T>
        requires std::assignable_from<InterfacePtr&, T&&>
        constexpr void setImplementation(T&& p)
        {
            p->injectRemotes(*this);
            impl = std::forward<T>(p);
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
        InterfacePtr impl = nullptr;
    };
};

DI_MODULE_EXPORT
struct IRemotes
{
    using Types = EmptyTypes;
    using Traits = TraitsOpen<IRemotes>;

    virtual ~IRemotes() = default;

    template<class Self>
    constexpr auto asTrait(this Self& self, auto trait, auto... keys)
    {
        return TraitView(trait, Alias(self, keys...), std::type_identity<typename Self::Types>{});
    }

private:
    friend struct INode;
    virtual void remotesRelocated(INode*, void*) = 0;
};

DI_MODULE_EXPORT
struct INode
{
    using Types = EmptyTypes;
    using Traits = TraitsOpen<INode>;

    virtual ~INode() = default;

    using Remotes = void;

    template<class Self, class Virtual>
    constexpr void injectRemotes(this Self& self, Virtual& virtualNode)
    {
        using Remotes = Self::Remotes;
        if (self.remotes)
        {
            self.remotes->remotesRelocated(std::addressof(self), std::addressof(virtualNode));
        }
        else if constexpr (not std::is_void_v<Remotes>)
        {
            static_assert(std::derived_from<Remotes, IRemotes>);
            using RemotesImpl = Self::template RemotesImpl<Virtual>;
            static_assert(std::derived_from<RemotesImpl, Remotes>);
            static_assert(std::is_final_v<RemotesImpl>);
            self.remotes = std::make_unique<RemotesImpl>(Alias(virtualNode));
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
template<IsNode Node, std::derived_from<IRemotes> RemotesBase>
struct IRemotesImpl : RemotesBase
{
    explicit constexpr IRemotesImpl(Alias<Node> node) : node(node) {}

protected:
    using Base = IRemotesImpl;

    template<IsTrait Trait, class Key = ContextOf<Node>::Info::DefaultKey>
    constexpr auto getNode(this auto& self, Trait trait, Key key = {})
    {
        return self.node->getNode(trait, key);
    }

private:
    constexpr void remotesRelocated(INode*, void* n)
    {
        node = Alias(*static_cast<Node*>(n));
    }
    Alias<Node> node;
};

DI_MODULE_EXPORT
template<IsNode Node, std::derived_from<INode> InterfaceBase_>
struct INodeImpl : InterfaceBase_
{
    using InterfaceBase = InterfaceBase_;

    explicit INodeImpl(Alias<Node> node) : node(node)
    {}

    struct Remotes final : IRemotes
    {
        // No need to keep alias in remotes, as INodeImpl already has the alias
        explicit Remotes(Alias<Node>) {}

        // Only functionality really needed by INodeImpl
        constexpr void remotesRelocated(INode* nodeImpl, void* node)
        {
            dynamic_cast<INodeImpl*>(nodeImpl)->node = Alias(*static_cast<Node*>(node));
        }
    };
    template<class>
    using RemotesImpl = Remotes;

protected:
    using Base = INodeImpl;

    template<IsTrait Trait, class Key = ContextOf<Node>::Info::DefaultKey>
    auto getNode(this auto& self, Trait trait, Key key = {})
    {
        return self.node->getNode(trait, key);
    }

private:
    Alias<Node> node;
};

DI_MODULE_EXPORT
template<template<class> class INodeImpl, template<class...> class Pointer = std::unique_ptr>
struct INodeFactory
{
    template<class Virtual>
    constexpr void injectRemotes(Virtual& v)
    {
        using Impl = INodeImpl<Virtual>;
        static_assert(std::derived_from<Impl, INode>);
        static_assert(std::derived_from<typename Impl::InterfaceBase, INode>);
        static_assert(std::is_final_v<Impl>);
        auto impl = Pointer<Impl>(new Impl(Alias(v)));
        impl->injectRemotes(v);
        p = std::move(impl);
    }

    constexpr INodeFactory* operator->() { return this; }

    template<std::derived_from<INode> T>
    constexpr operator Pointer<T>() &&
    {
        return Pointer<T>(dynamic_cast<T*>(p.release()));
    }

private:
    Pointer<INode> p;
};

} // namespace di


#endif // INCLUDE_DI_VIRTUAL_HPP
