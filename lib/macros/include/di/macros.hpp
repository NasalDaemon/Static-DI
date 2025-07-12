#ifndef INCLUDE_DI_MACROS_HPP
#define INCLUDE_DI_MACROS_HPP

#ifndef DI_MODULE_EXPORT
#   define DI_MODULE_EXPORT
#endif

#define DI_CAT(l, r) l ## r
#define DI_JOIN(l, r) l r
#define DI_APPLY(macro, ...) DI_JOIN(macro, (__VA_ARGS__))
#define DI_DEPAREN(X) DI_ESC(DI_ISH X)
#define DI_ISH(...) DI_ISH __VA_ARGS__
#define DI_ESC(...) DI_ESC_(__VA_ARGS__)
#define DI_ESC_(...) DI_VANISH_ ## __VA_ARGS__
#define DI_VANISH_DI_ISH

// Count number of arguments
#define DI_ARGC(...) DI_APPLY( \
    DI_ARGC_,__VA_ARGS__, 63,62,61,60, \
        59,58,57,56,55,54,53,52,51,50, \
        49,48,47,46,45,44,43,42,41,40, \
        39,38,37,36,35,34,33,32,31,30, \
        29,28,27,26,25,24,23,22,21,20, \
        19,18,17,16,15,14,13,12,11,10, \
         9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define DI_ARGC_( \
     _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
    _61,_62,_63,N,...) N

#define DI_OVERLOAD(macro, ...) DI_APPLY(DI_CAT, macro, DI_ARGC(__VA_ARGS__))

// DI_MAKE_VERSION(major, minor=0, patch=0)
#define DI_MAKE_VERSION(...) DI_OVERLOAD(DI_MAKE_VERSION, __VA_ARGS__)(__VA_ARGS__)

// MMM|NNN|PPPPP
#define DI_MAKE_VERSION3(major, minor, patch) ((1'000ull * 100'000ull * (major)) + (100'000ull * (minor)) + (patch))
#define DI_MAKE_VERSION2(major, minor)        DI_MAKE_VERSION3(major, minor, 0)
#define DI_MAKE_VERSION1(major)               DI_MAKE_VERSION3(major, 0, 0)

// Prefer using C++ wrappers in di::compiler
#if defined(__clang__)
#   define DI_COMPILER_IS_X(clang, ...) DI_DEPAREN(clang)
#   define DI_COMPILER_VERSION DI_MAKE_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
#elif defined(__GNUC__) || defined(__GNUG__)
#   define DI_COMPILER_IS_X(clang, gcc, ...) DI_DEPAREN(gcc)
#   define DI_COMPILER_VERSION DI_MAKE_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
#   define DI_COMPILER_IS_X(clang, gcc, msvc, ...) DI_DEPAREN(msvc)
// _MSC_VER MM|NN
// _MSC_FULL_VER MM|NN|PPPPP
#   define DI_COMPILER_VERSION DI_MAKE_VERSION(_MSC_VER / 100, _MSC_VER % 100, _MSC_FULL_VER % 100'000)
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

// DI_COMPILER_GE(GCC, major, minor=0, patch=0)
#define DI_COMPILER_GE(NAME, ...) DI_COMPILER_CMP(NAME, >=, __VA_ARGS__)
// DI_COMPILER_LT(GCC, major, minor=0, patch=0)
#define DI_COMPILER_LT(NAME, ...) DI_COMPILER_CMP(NAME, <, __VA_ARGS__)

#define DI_COMPILER_CMP(NAME, op, ...) (DI_CAT(DI_COMPILER_, NAME) && (DI_COMPILER_VERSION op DI_MAKE_VERSION(__VA_ARGS__)))

#if DI_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#if DI_COMPILER_MSVC
#   define DI_CPP_VERSION _MSVC_LANG
#else
#   define DI_CPP_VERSION __cplusplus
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

#if DI_COMPILER_MSVC
#   define DI_INLINE [[msvc::forceinline]] inline
#else
#   define DI_INLINE [[gnu::always_inline, gnu::artificial]] inline
#endif

// Clang won't detail the failure in the build diagnostic when asserting the concept directly
#if DI_COMPILER_CLANG
#   define DI_ASSERT_IMPLEMENTS(Impl, Types, Trait) \
    static_assert(::di::detail::alwaysTrue<::di::detail::Implements<Impl, Types, Trait>>)
#else
#   define DI_ASSERT_IMPLEMENTS(Impl, Types, Trait) \
    static_assert(::di::Implements<Impl, Types, Trait>)
#endif

#define DI_FWD(...) DI_OVERLOAD(DI_FWD, __VA_ARGS__)(__VA_ARGS__)

#define DI_FWD1(name)    DI_FWD2(decltype(name), name)
#define DI_FWD2(T, name) static_cast<T&&>(name)

// DI_METHODS(TraitName, MethodList=DI_METHODS_TraitName)
#define DI_METHODS(...) DI_OVERLOAD(DI_METHODS, __VA_ARGS__)(__VA_ARGS__)

#define DI_METHODS1(traitName) \
    DI_METHODS2(traitName, DI_METHODS_ ## traitName)

#define DI_METHODS2(traitName, METHOD_LIST) \
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
    DI_INLINE constexpr decltype(auto) method(this Self&& self, ::di::AsFunctor asFunctor) \
    { \
        return self.impl(method ## _c, asFunctor); \
    }

#define DI_DUCK_METHOD(method) \
    template<::di::IsTraitView Self> \
    DI_INLINE constexpr decltype(auto) method(this Self&& self, auto&&... args) \
    { \
        return self.impl(method ## _c, DI_FWD(args)...); \
    }

// DI_LINK(TraitName, TargetContext, TargetTraitRename=<NoRename>)
#define DI_LINK(...) DI_OVERLOAD(DI_LINK, __VA_ARGS__)(__VA_ARGS__)

#define DI_LINK2(traitName, targetContext) \
    DI_LINK3(traitName, targetContext, T)

#define DI_LINK3(traitName, targetContext, targetTrait) \
    template<::di::MatchesTrait<DI_DEPAREN(traitName)> T> \
    static ::di::ResolvedLink<DI_DEPAREN(targetContext), DI_DEPAREN(targetTrait)> resolveLink(T);

#define DI_NODE(Context, nodeName, ... /* predicates */) \
    [[no_unique_address]] ::di::Ensure<::di::ContextToNodeState<Context>, ## __VA_ARGS__> nodeName{}; \
    friend consteval auto getNodePointer(di::AdlTag<Context>) { return &Context::Parent::nodeName; }

#define DI_INSTANTIATE(graph, dotPath) \
    template struct std::remove_cvref_t<decltype(DI_DEPAREN(graph)::dotPath)>::Node<::di::ContextOf<std::remove_cvref_t<decltype(DI_DEPAREN(graph)::dotPath)>>>;

#define DI_INSTANTIATE_BOX(Main, InFacade, ... /* interfaces */) \
    DI_INSTANTIATE((::di::Box<Main, InFacade, ## __VA_ARGS__>::Graph), main)

// used in main.cpp
#define DI_CONSTRUCT(...) \
    ::di::constructGraph([&]() -> decltype(auto) \
    { \
        return (__VA_ARGS__); \
    })

#if DI_COMPILER_CLANG
#pragma clang diagnostic pop
#endif

#endif // INCLUDE_DI_MACROS_HPP
