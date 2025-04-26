#ifndef INCLUDE_DI_ALIAS_HPP
#define INCLUDE_DI_ALIAS_HPP

#include "di/detail/compress.hpp"

#include "di/macros.hpp"

#if !DI_STD_MODULE
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

    constexpr explicit Alias(Impl& impl, Key...) : impl(std::addressof(impl)) {}

    template<class Self>
    constexpr auto& get(this Self&& self) { return std::forward_like<Self&>(*self.impl); }
    constexpr auto* operator->(this auto&& self) { return std::addressof(self.get()); }

private:
    Impl* impl;
};

template<class T, class Key>
struct Alias<T, Key> final
{
    using Impl = T;
    using Interface = Alias;
    using Traits = Impl::Traits;

    constexpr Alias(auto& impl, Key key) : impl(impl), key(key) {}

    constexpr auto& get(this auto&& self) { return self; }
    constexpr auto* operator->(this auto&& self) { return std::addressof(self); }

    constexpr auto apply(this auto&& self, auto&&... args)
        -> decltype(self.impl->applyWithKey(self.key, DI_FWD(args)...))
    {
        return self.impl->applyWithKey(self.key, DI_FWD(args)...);
    }

    constexpr decltype(auto) visit(this auto&& self, auto&& visitor)
    {
        return self.impl->visit(DI_FWD(visitor));
    }

private:
    Alias<Impl> impl;
    [[no_unique_address]] Key key{};
};

template<class T, class... Key>
Alias(T&, Key...) -> Alias<T, Key...>;

DI_MODULE_EXPORT
constexpr auto makeAlias(auto& impl, auto... key) { return Alias(detail::compressImpl(impl), key...); }

} // namespace di


#endif // INCLUDE_DI_ALIAS_HPP
