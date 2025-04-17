#ifndef INCLUDE_DI_MACROS_HPP
#define INCLUDE_DI_MACROS_HPP

// When used as a header library (not as a module)
#ifndef DI_MODULE_EXPORT
#   define DI_HEADER 1
#   define DI_MODULE_EXPORT
#endif

#if defined(__clang__)
#   define DI_COMPILER_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
#   define DI_COMPILER_GCC 1
#elif defined(_MSC_VER)
#   define DI_COMPILER_MSVC 1
#else
#   error Unsupported compiler
#endif

#if defined(__INTELLISENSE__) or defined(DI_CLANGD)
#   define DI_AUTOCOMPLETE 1
#endif

// Clang won't detail the failure in the build diagnostic when asserting the concept directly
#if DI_COMPILER_CLANG
#   define DI_ASSERT_IMPLEMENTS(Impl, Types, Trait) \
    static_assert(::di::detail::alwaysTrue<::di::detail::Implements<Impl, Types, Trait>>)
#else
#   define DI_ASSERT_IMPLEMENTS(Impl, Types, Trait) \
    static_assert(::di::Implements<Impl, Types, Trait>)
#endif

#define DI_FWD(name) static_cast<decltype(name)&&>(name)

#define DI_DEPAREN(X) DI_ESC(DI_ISH X)
#define DI_ISH(...) DI_ISH __VA_ARGS__
#define DI_ESC(...) DI_ESC_(__VA_ARGS__)
#define DI_ESC_(...) DI_VANISH_ ## __VA_ARGS__
#define DI_VANISH_DI_ISH

// used in traits.hpp
#define DI_METHODS(traitName) \
    DI_METHODS_EX(traitName, traitName ## _DI_METHODS)

#define DI_METHODS_EX(traitName, METHOD_LIST) \
    METHOD_LIST(DI_METHOD_TAG) \
    friend constexpr traitName traitOf(::di::IsMethodOf<traitName> auto) { return {}; } \
    struct Meta \
    { \
        struct Applicable \
        { \
            METHOD_LIST(DI_METHOD_TAG_APPLICABLE) \
        }; \
        struct Methods \
        { \
            METHOD_LIST(DI_DUCK_METHOD) \
        }; \
        using DuckMethods = Methods; \
    };

#define DI_METHOD_TAG(method) \
    struct method{} static constexpr method ## _c{};

#define DI_METHOD_TAG_APPLICABLE(method) \
    static void applicable(method);

#define DI_AS_FUNCTOR_METHOD(method) \
    template<::di::IsTraitView Self> \
    constexpr decltype(auto) method(this Self&& self, ::di::AsFunctor asFunctor) \
    { \
        return self.apply(method ## _c, asFunctor); \
    }

#define DI_DUCK_METHOD(method) \
    template<::di::IsTraitView Self> \
    constexpr decltype(auto) method(this Self&& self, auto&&... args) \
    { \
        return self.apply(method ## _c, DI_FWD(args)...); \
    }

// used in cluster.hpp
#define DI_LINK(traitName, targetContext) \
    template<::di::MatchesTrait<DI_DEPAREN(traitName)> T> \
    static ::di::ResolvedLink<DI_DEPAREN(targetContext), T> resolveLink(T);

#define DI_LINK_TO(traitName, targetContext, targetTrait) \
    template<::di::MatchesTrait<DI_DEPAREN(traitName)> T> \
    static ::di::ResolvedLink<DI_DEPAREN(targetContext), DI_DEPAREN(targetTrait)> resolveLink(T);

#define DI_NODE(Context, nodeName) \
    [[no_unique_address]] ::di::ContextToNodeState<Context> nodeName{}; \
    friend consteval auto getNodePointer(di::AdlTag<Context>) { return &Context::Parent::nodeName; }

// used in node.cpp
#define DI_INSTANTIATE(graph, dotPath) \
    template struct std::remove_cvref_t<decltype(graph::dotPath)>::Node<::di::ContextOf<std::remove_cvref_t<decltype(graph::dotPath)>>>;

// used in main.cpp
#define DI_CONSTRUCT(...) \
    ::di::constructGraph([&] \
    { \
        return __VA_ARGS__; \
    })

#endif // INCLUDE_DI_MACROS_HPP
