#ifndef INCLUDE_DI_MACROS_HPP
#define INCLUDE_DI_MACROS_HPP

// When used as a header library (not as a module)
#ifndef DI_MODULE_EXPORT
#   define DI_HEADER 1
#   define DI_MODULE_EXPORT
#endif

#define DI_DEPAREN(X) DI_ESC(DI_ISH X)
#define DI_ISH(...) DI_ISH __VA_ARGS__
#define DI_ESC(...) DI_ESC_(__VA_ARGS__)
#define DI_ESC_(...) DI_VANISH_ ## __VA_ARGS__
#define DI_VANISH_DI_ISH

// MMM|NNN|PPPPP
#define DI_MAKE_VER(major, minor, patch) \
    ((1'000ull * 100'000ull * (major)) + (100'000ull * (minor)) + (patch))

#if defined(__clang__)
#   define DI_COMPILER_IS_X(clang, ...) DI_DEPAREN(clang)
#   define DI_COMPILER_VER DI_MAKE_VER(__clang_major__, __clang_minor__, __clang_patchlevel__)
#elif defined(__GNUC__) || defined(__GNUG__)
#   define DI_COMPILER_IS_X(clang, gcc, ...) DI_DEPAREN(gcc)
#   define DI_COMPILER_VER DI_MAKE_VER(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
#   define DI_COMPILER_IS_X(clang, gcc, msvc, ...) DI_DEPAREN(msvc)
// _MSC_VER MM|NN
// _MSC_FULL_VER MM|NN|PPPPP
#   define DI_COMPILER_VER DI_MAKE_VER(_MSC_VER / 100, _MSC_VER % 100, _MSC_FULL_VER % 100'000)
#else
#   error Unsupported compiler
#endif

//                                              CLANG          GCC            MSVC
#define DI_IF_CLANG(...)      DI_COMPILER_IS_X((__VA_ARGS__), (           ), (           ))
#define DI_IF_GCC(...)        DI_COMPILER_IS_X((           ), (__VA_ARGS__), (           ))
#define DI_IF_GNU(...)        DI_COMPILER_IS_X((__VA_ARGS__), (__VA_ARGS__), (           ))
#define DI_IF_MSVC(...)       DI_COMPILER_IS_X((           ), (           ), (__VA_ARGS__))
#define DI_IF_NOT_CLANG(...)  DI_COMPILER_IS_X((           ), (__VA_ARGS__), (__VA_ARGS__))
#define DI_IF_NOT_GCC(...)    DI_COMPILER_IS_X((__VA_ARGS__), (           ), (__VA_ARGS__))
#define DI_IF_NOT_GNU(...)    DI_COMPILER_IS_X((           ), (           ), (__VA_ARGS__))
#define DI_IF_NOT_MSVC(...)   DI_COMPILER_IS_X((__VA_ARGS__), (__VA_ARGS__), (           ))

// Calling convention: DI_IF_CLANG_ELSE(IF_CLANG...)(ELSE...)
#define DI_IF_CLANG_ELSE(...)  DI_IF_CLANG(__VA_ARGS__) DI_IF_NOT_CLANG
#define DI_IF_GCC_ELSE(...)    DI_IF_GCC  (__VA_ARGS__) DI_IF_NOT_GCC
#define DI_IF_GNU_ELSE(...)    DI_IF_GNU  (__VA_ARGS__) DI_IF_NOT_GNU
#define DI_IF_MSVC_ELSE(...)   DI_IF_MSVC (__VA_ARGS__) DI_IF_NOT_MSVC

#define DI_COMPILER_CLANG DI_IF_CLANG_ELSE(1)(0)
#define DI_COMPILER_GCC   DI_IF_GCC_ELSE(1)(0)
#define DI_COMPILER_GNU   DI_IF_GNU_ELSE(1)(0)
#define DI_COMPILER_MSVC  DI_IF_MSVC_ELSE(1)(0)

#if DI_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#if DI_COMPILER_MSVC
#   define DI_CPP_VER _MSVC_LANG
#else
#   define DI_CPP_VER __cplusplus
#endif

#if defined(_WIN32)
#   define DI_OS_WINDOWS 1
#else
#   define DI_OS_WINDOWS 0
#endif

#if defined(__INTELLISENSE__) or defined(DI_CLANGD)
#   define DI_AUTOCOMPLETE 1
#else
#   define DI_AUTOCOMPLETE 0
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

// used in traits.hpp
#define DI_METHODS(traitName) \
    DI_METHODS_EX(traitName, DI_METHODS_ ## traitName)

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

#define DI_NODE(Context, nodeName, ... /* predicates */) \
    [[no_unique_address]] ::di::Ensure<::di::ContextToNodeState<Context>, ## __VA_ARGS__> nodeName{}; \
    friend consteval auto getNodePointer(di::AdlTag<Context>) { return &Context::Parent::nodeName; }

// used in node.cpp
#define DI_INSTANTIATE(graph, dotPath) \
    template struct std::remove_cvref_t<decltype(DI_DEPAREN(graph)::dotPath)>::Node<::di::ContextOf<std::remove_cvref_t<decltype(DI_DEPAREN(graph)::dotPath)>>>;

#define DI_INSTANTIATE_BOX(Main, InFacade, ... /* interfaces */) \
    DI_INSTANTIATE((::di::Box<Main, InFacade, ## __VA_ARGS__>::Graph), main)

// used in main.cpp
#define DI_CONSTRUCT(...) \
    ::di::constructGraph([&] \
    { \
        return __VA_ARGS__; \
    })

#if DI_COMPILER_CLANG
#pragma clang diagnostic pop
#endif

#endif // INCLUDE_DI_MACROS_HPP
