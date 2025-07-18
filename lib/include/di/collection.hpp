#include "di/detail/as_ref.hpp"
#include "di/detail/cast.hpp"

#include "di/context_fwd.hpp"
#include "di/empty_types.hpp"
#include "di/key.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/peer.hxx"
#include "di/traits.hpp"

#if !DI_IMPORT_STD
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <ranges>
#include <type_traits>
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
        struct Element
        {
            ID id;
            Node* collection;
            [[no_unique_address]] ElementNode node;

            explicit Element(ID const& id, Node* collection, auto&&... args)
                : id(id), collection(collection), node(DI_FWD(args)...)
            {}

            template<class Caller, class Target>
            constexpr auto getPeers(Target Element::* elToNodeMemPtr) const
            {
                using CallerNode = Caller::Traits::Node;
                return std::as_const(collection->elements)
                    | std::views::filter(
                        [=, this](auto const& el)
                        {
                            if (&el == this)
                                return false;
                            auto const peer = (el.*elToNodeMemPtr).asTrait(trait::peer);
                            if (not peer.isPeerId(id))
                                return false;
                            auto const& instance = detail::downCast<Caller>(detail::upCast<CallerNode>(this->*elToNodeMemPtr));
                            return peer.isPeerInstance(instance);
                        })
                    | std::views::transform(
                        [=](auto const& el) -> auto const&
                        {
                            return detail::downCast<Caller>(detail::upCast<CallerNode>(el.*elToNodeMemPtr));
                        });
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

            constexpr auto getPeerMemPtr()
            {
                return &Element::node;
            }

            using IdType = ID;

            struct Info : Context::Info
            {
                using CollectionContext = ElementContext;
            };

        private:
            static constexpr auto const& getElement(auto& node)
            {
                return detail::downCast<ElementNode>(node).*detail::reverseMemberPointer(&Element::node);
            }
            template<class InnerNode>
            static constexpr auto const& getCollection(InnerNode& node)
            {
                return *getElement(node).collection;
            }
        };

        template<class>
        struct AsTrait;

        template<class Trait>
        requires HasTrait<ElementNode, Trait>
        using TraitsTemplate = di::ResolvedTrait<AsTrait<Trait>, typename detail::ResolveTrait<ElementNode, Trait>::type::Types>;

        std::vector<Element> elements;

        constexpr void add(ID const& id, auto&&... args)
        {
            // Nodes must not be invalidated by insertions, so the vector must not be resized.
            if (elements.capacity() == elements.size())
                throw std::length_error("Collection capacity exceeded");
            if (getId(id) != nullptr)
                throw std::invalid_argument("ID already exists in collection");
            elements.emplace_back(id, this, DI_FWD(args)...);
        }

    public:
        constexpr explicit Node(std::size_t capacity, auto adder)
        {
            elements.reserve(capacity);
            adder([this](ID const& id, auto&&... args) { this->add(id, DI_FWD(args)...); });
        }

        constexpr Node(Node const& other)
            : elements(other.elements)
        {
            for (auto& element : elements)
                element.collection = this;
        }
        constexpr Node(Node&& other)
            : elements(std::move(other.elements))
        {
            for (auto& element : elements)
                element.collection = this;
        }

        using Traits = di::TraitsTemplate<Node, TraitsTemplate>;

        constexpr auto* getId(this auto& self, ID const& id)
        {
            auto const it = std::find_if(
                self.elements.begin(),
                self.elements.end(),
                [&](auto const& element) { return element.id == id; });
            return it != self.elements.end()
                ? std::addressof(it->node)
                : nullptr;
        }
    };
};

template<class ID, IsNodeHandle NodeHandle>
template<class Context>
template<class Trait>
struct Collection<ID, NodeHandle>::Node<Context>::AsTrait : Node
{
    template<class Source>
    constexpr auto finalize(this auto& self, Source& source, key::Element<ID> key)
    {
        auto* const element = self.getId(key.id);
        if (element == nullptr) [[unlikely]]
            throw std::out_of_range("Element with given ID does not exist in collection");
        auto target = element->asTrait(detail::AsRef{}, Trait{});
        return target.ptr->finalize(source, key::Default{});
    }

    template<class Source, class P>
    constexpr auto finalize(this auto& self, Source&, key::Elements<P> const& key)
    {
        using Environment = Source::Environment;
        return makeAlias(withEnv<Environment>(detail::downCast<WithPred<P>>(detail::upCast<Node>(self))), key.pred);
    }

    template<class Pred>
    struct WithPred : Node
    {
        template<class Self, class... Args>
        constexpr void implWithKey(this Self& self, Pred const& pred, Args&&... args)
        {
            for (auto& el : self.elements)
            {
                if (pred(std::as_const(el.id)))
                {
                    auto target = el.node.asTrait(detail::AsRef{}, Trait{});
                    target.ptr->finalize(self, key::Default{})->impl(DI_FWD(args)...);
                }
            }
        }
    };
};

}
