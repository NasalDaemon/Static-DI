#ifndef INCLUDE_DI_ALIAS_HPP
#define INCLUDE_DI_ALIAS_HPP

#include "di/detail/compress.hpp"

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <memory>
#include <utility>
#endif

namespace di {

DI_MODULE_EXPORT
template<class T, class... Key>
struct Alias final
{
    static_assert(sizeof...(Key) == 0);

    using Impl = T;
    using Interface = T;

    explicit constexpr Alias(Impl& impl, Key...) : impl(std::addressof(impl)) {}

    template<class Self>
    DI_INLINE constexpr auto& get(this Self&& self) { return std::forward_like<Self&>(*self.impl); }
    DI_INLINE constexpr auto* operator->(this auto&& self) { return std::addressof(self.get()); }

private:
    Impl* impl;
};

template<class T, class Key>
struct Alias<T, Key> final
{
    using Impl = T;
    using Interface = Alias;
    using Traits = Impl::Traits;

    constexpr Alias(auto& alias, Key key) : alias(alias), key(key) {}

    constexpr auto& get(this auto&& self) { return self; }
    constexpr auto* operator->(this auto&& self) { return std::addressof(self); }

    DI_INLINE constexpr auto impl(this auto&& self, auto&&... args)
        -> decltype(self.alias->implWithKey(self.key, DI_FWD(args)...))
    {
        return self.alias->implWithKey(self.key, DI_FWD(args)...);
    }

    DI_INLINE constexpr decltype(auto) visit(this auto&& self, auto&& visitor)
    {
        return self.alias->visit(DI_FWD(visitor));
    }

private:
    Alias<Impl> alias;
    [[no_unique_address]] Key key{};
};

template<class T, class... Key>
Alias(T&, Key...) -> Alias<T, Key...>;

DI_MODULE_EXPORT
constexpr auto makeAlias(auto& impl, auto... key) { return Alias(detail::compressImpl(impl), key...); }

} // namespace di


#endif // INCLUDE_DI_ALIAS_HPP
