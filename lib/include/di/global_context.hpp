#ifndef INCLUDE_DI_GLOBAL_CONTEXT_HPP
#define INCLUDE_DI_GLOBAL_CONTEXT_HPP

#include "di/context_fwd.hpp"
#include "di/detail/cast.hpp"
#include "di/detail/compress.hpp"
#include "di/global_trait.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/trait_view.hpp"

namespace di {

DI_MODULE_EXPORT
template<class Context>
concept ContextHasGlobal = IsContext<Context> and requires {
    typename Context::Info::GlobalNode;
};

DI_MODULE_EXPORT
template<class Context, class GlobalTrait>
concept ContextHasGlobalTrait = ContextHasGlobal<Context> and IsGlobalTrait<GlobalTrait> and HasTrait<typename Context::Info::GlobalNode, typename GlobalTrait::Trait>;

namespace detail {

    template<IsContext Context, IsGlobalTrait GlobalTrait>
    consteval void assertContextHasGlobalTrait()
    {
        static_assert(ContextHasGlobal<Context>, "Context does not have a global type");
        static_assert(ContextHasGlobalTrait<Context, GlobalTrait>, "Global type does not have the requested trait");
    }

    template<class Context>
    struct GlobalNodePtr
    {
        constexpr GlobalNodePtr(void*) {}

        static void* get() { return nullptr; }
        static void set(void*) {}
    };

    template<ContextHasGlobal Context>
    struct GlobalNodePtr<Context>
    {
        using Node = ContextToNode<Decompress<Context>>;
        using GlobalNode = Context::Info::GlobalNode;
        constexpr GlobalNodePtr(Node* node)
        {
            set(node);
        }

        DI_INLINE constexpr auto* get() const { return ptr; }
        DI_INLINE constexpr void set(Node* node) { ptr = std::addressof(upCast<GlobalNode>(Context{}.getGlobalNode(*node))); }
    private:
        GlobalNode* ptr = nullptr;
    };
}

} // namespace di


#endif // INCLUDE_DI_GLOBAL_CONTEXT_HPP
