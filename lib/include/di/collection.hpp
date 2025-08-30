#include "di/adapt.hpp"
#include "di/detail/as_ref.hpp"
#include "di/detail/cast.hpp"

#include "di/context_fwd.hpp"
#include "di/detail/compress.hpp"
#include "di/detail/concepts.hpp"
#include "di/empty_types.hpp"
#include "di/environment.hpp"
#include "di/finalise.hpp"
#include "di/global_context.hpp"
#include "di/global_trait.hpp"
#include "di/key.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/traits/peer.hxx"
#include "di/traits.hpp"

#if !DI_IMPORT_STD
#include <algorithm>
#include <iterator>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#endif

namespace di {

namespace detail {
    struct AlwaysTruePred
    {
        constexpr std::true_type operator()(auto&&...) const { return {}; }
    };
}

namespace key {
    DI_MODULE_EXPORT
    template<class ID>
    struct Element : key::Default
    {
        explicit Element(ID id) : id(id) {}
        ID id;
    };

    template<class ID>
    Element(ID) -> Element<ID>;

    DI_MODULE_EXPORT
    template<class Predicate>
    struct Elements : key::Default
    {
        template<class T>
        using Trait = detail::DuckTrait<T>;

        Elements() = default;
        explicit Elements(Predicate pred) : pred(std::move(pred)) {}

        [[no_unique_address]] Predicate pred{};
    };

    template<class Pred>
    Elements(Pred) -> Elements<Pred>;

    DI_MODULE_EXPORT
    inline constexpr Elements<detail::AlwaysTruePred> allElements{};
}

DI_MODULE_EXPORT
template<class ID, IsNodeHandle NodeHandle>
struct Collection
{
    template<class Context>
    class Node : public di::Node
    {
        struct ElementContext;

        using ElementNode = detail::ToNodeState<typename ToNodeWrapper<NodeHandle>::template Node<detail::CompressContext<ElementContext>>>;

        struct Handle
        {
            Handle() = delete;
            auto operator<=>(Handle const&) const = default;
        private:
            friend Node;
            constexpr explicit Handle(std::size_t index) : index(index) {}
            std::size_t index;
        };

        struct Element
        {
            ID id;
            Node* collection;
            [[no_unique_address]] detail::GlobalNodePtr<Context> globalNode;
            [[no_unique_address]] ElementNode node;

            explicit Element(ID const& id, Node* collection, auto&&... args)
                : id(id), collection(collection), globalNode(collection), node(DI_FWD(args)...)
            {}

            template<class Caller, class Target>
            constexpr auto getPeers(detail::MemberPtr<Element, Target> elToNodeMemPtr) const
            {
                using CallerNode = Caller::Traits::Node;
                using TargetNode = Target::Traits::Node;
                // Remove dynamic environment components from Caller, as the peers are independent instances
                using Environment = Caller::Environment::RemoveDynamic;
                using NodeState = TransferEnv<Environment, di::ContextToNodeState<detail::Decompress<ContextOf<Caller>>>>;
                static_assert(std::is_same_v<CallerNode, TargetNode>);
                return std::as_const(collection->elements)
                    | std::views::filter(
                        [=, this](auto const& el) -> bool
                        {
                            if (&el == this)
                                return false;
                            auto const peer = elToNodeMemPtr.getMemberFromClass(el).asTrait(trait::peer);
                            if (not peer.isPeerId(id))
                                return false;
                            auto const& instance = detail::downCast<NodeState>(detail::upCast<CallerNode>(elToNodeMemPtr.getMemberFromClass(*this)));
                            return peer.isPeerInstance(instance);
                        })
                    | std::views::transform(
                        [=](auto const& el) -> NodeState const&
                        {
                            return detail::downCast<NodeState>(detail::upCast<CallerNode>(elToNodeMemPtr.getMemberFromClass(el)));
                        });
            }

            constexpr Handle getElementHandle() const
            {
                return Handle(this - std::to_address(collection->elements.begin()));
            }
        };

        struct ElementContext : Context
        {
            template<IsTrait Trait>
            requires detail::HasLink<Context, Trait>
            static constexpr auto getNode(auto& node, Trait)
            {
                return Context{}.getNode(getCollection(node), detail::ResolveLinkTrait<Context, Trait>{});
            }

            template<IsGlobalTrait GlobalTrait>
            static constexpr auto getNode(auto& node, GlobalTrait)
            {
                detail::assertContextHasGlobalTrait<ElementContext, GlobalTrait>();
                return getGlobalNode(node).asTrait(detail::AsRef{}, typename GlobalTrait::Trait{});
            }

            template<class N>
            static constexpr auto& getGlobalNode(N& node)
            {
                // Global node pointer is cached in Element to avoid multiple lookups
                return std::forward_like<N&>(*getElement(node).globalNode.get());
            }

            template<IsContext Parent>
            constexpr auto getParentMemPtr()
            {
                static_assert(std::is_same_v<Parent, ElementContext>, "di::Collection does not have a stable member pointer relative to its parent node");
                return DI_MEM_PTR(Element, node);
            }

            template<IsContext Parent>
            constexpr auto& getParentNode(auto& node)
            {
                if constexpr (std::is_same_v<detail::Decompress<Parent>, ElementContext>)
                    return node;
                else
                    return Context{}.template getParentNode<Parent>(getCollection(node));
            }

            using IdType = ID;
            using CollectionContext = Context;

            struct Info : Context::Info
            {
                using ElementContext = Node::ElementContext;
            };

        private:
            static constexpr auto& getElement(auto& node)
            {
                auto memPtr = DI_MEM_PTR(Element, node);
                return memPtr.getClassFromMember(node);
            }
            template<class InnerNode>
            static constexpr auto& getCollection(InnerNode& node)
            {
                return std::forward_like<InnerNode&>(*getElement(node).collection);
            }
        };

        template<class>
        struct AsTrait;

        template<class Trait>
        requires HasTrait<ElementNode, Trait>
        using TraitsTemplate = di::ResolvedTrait<AsTrait<Trait>, typename detail::ResolveTrait<ElementNode, Trait>::Types>;

        std::vector<Element> elements;
        std::vector<ID> ids;

        constexpr void add(ID const& id, auto&&... args)
        {
            // Nodes must not be invalidated by insertions, so the vector must not be resized.
            if (elements.capacity() == elements.size())
                throw std::length_error("Collection capacity exceeded");
            if (hasId(id))
                throw std::invalid_argument("ID already exists in collection");
            ids.push_back(id);
            elements.emplace_back(id, this, DI_FWD(args)...);
        }

    public:
        constexpr explicit Node(std::size_t capacity, auto adder)
        {
            elements.reserve(capacity);
            ids.reserve(capacity);
            adder([this](ID const& id, auto&&... args) { this->add(id, DI_FWD(args)...); });
        }

        constexpr Node(Node const& other)
            : elements(other.elements)
            , ids(other.ids)
        {
            for (auto& element : elements)
            {
                element.collection = this;
                element.globalNode.set(this);
            }
        }
        constexpr Node(Node&& other)
            : elements(std::move(other.elements))
            , ids(std::move(other.ids))
        {
            for (auto& element : elements)
            {
                element.collection = this;
                element.globalNode.set(this);
            }
        }

        using Traits = di::TraitsTemplate<Node, TraitsTemplate>;

        [[nodiscard]] constexpr bool hasId(ID const& id) const
        {
            return std::find(ids.begin(), ids.end(), id) != ids.end();
        }

        [[nodiscard]] constexpr auto* getId(this auto& self, ID const& id)
        {
            auto const it = std::find(self.ids.begin(), self.ids.end(), id);
            return it != self.ids.end()
                ? std::addressof(self.elements[std::distance(self.ids.begin(), it)].node)
                : nullptr;
        }

        // Throws if ID does not exist in collection
        [[nodiscard]] constexpr auto& atId(this auto& self, ID const& id)
        {
            auto const it = std::find(self.ids.begin(), self.ids.end(), id);
            if (it == self.ids.end()) [[unlikely]]
                throw std::out_of_range("Element with given ID does not exist in collection");
            return self.elements[std::distance(self.ids.begin(), it)].node;
        }
    };
};

template<class ID, IsNodeHandle NodeHandle>
template<class Context>
template<class Trait>
struct Collection<ID, NodeHandle>::Node<Context>::AsTrait : Node
{
    constexpr auto finalise(this auto& self, auto& source, key::Element<ID> const& key, auto const&... keys)
    {
        auto* const element = self.getId(key.id);
        if (element == nullptr) [[unlikely]]
            throw std::out_of_range("Element with given ID does not exist in collection");
        auto target = element->asTrait(detail::AsRef{}, Trait{});
        return target.ptr->finalise(source, keys...);
    }

    constexpr auto finalise(this auto& self, auto& source, key::Element<Handle> const& key, auto const&... keys)
    {
        auto& element = self.elements[key.id.index].node;
        auto target = element.asTrait(detail::AsRef{}, Trait{});
        return target.ptr->finalise(source, keys...);
    }

    template<class T>
    constexpr auto finalise(this auto&, auto&, key::Element<T> const&, auto const&...)
    {
        static_assert(std::is_same_v<T, ID>, "T is not ID or Handle type");
    }

    template<class Self>
    DI_INLINE constexpr auto finalise(this Self& self, auto& source, auto const& key, auto const&... keys)
    {
        // Don't consume the key, as we need to consume it for each element
        return di::finalise<false>(source, self, key, keys...);
    }

    template<class Self, class Pred, class... Args>
    constexpr void implWithKey(this Self& self, key::Elements<Pred> const& key, auto const& keys, Args&&... args)
    {
        for (auto& el : self.elements)
        {
            if (key.pred(std::as_const(el.id)))
            {
                std::apply(
                    [&](auto const&... ks)
                    {
                        auto target = el.node.asTrait(detail::AsRef{}, Trait{});
                        target.ptr->finalise(self, ks...)->impl(DI_FWD(args)...);
                    },
                    keys);
            }
        }
    }
};

}
