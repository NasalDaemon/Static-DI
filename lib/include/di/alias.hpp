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
template<class T>
struct Alias final
{
    using Impl = T;
    using Interface = T;

    constexpr explicit Alias(T& ref) : ref(std::addressof(ref)) {}

    template<class Self>
    constexpr auto& get(this Self&& self) { return std::forward_like<Self&>(*self.ref); }

private:
    T* ref;
};

DI_MODULE_EXPORT
template<class T>
Alias(T&) -> Alias<T>;

DI_MODULE_EXPORT
template<class T, class Key>
struct AliasWithKey final
{
    using Impl = T;
    using Interface = AliasWithKey;
    using Traits = Impl::Traits;

    constexpr AliasWithKey(Impl& impl, Key key) : impl(impl), key(key) {}

    constexpr auto& get(this auto&& self) { return self; }

    constexpr auto apply(this auto&& self, auto&&... args)
        -> decltype(self.impl.get().applyWithKey(self.key, DI_FWD(args)...))
    {
        return self.impl.get().applyWithKey(self.key, DI_FWD(args)...);
    }

    constexpr decltype(auto) visit(this auto&& self, auto&& visitor)
    {
        return self.impl.get().visit(DI_FWD(visitor));
    }

    Alias<Impl> impl;
    [[no_unique_address]] Key key{};
};

DI_MODULE_EXPORT
template<class Impl, class Key>
AliasWithKey(Impl&, Key) -> AliasWithKey<Impl, Key>;

DI_MODULE_EXPORT
constexpr auto makeAlias(auto& impl) { return Alias(detail::compressImpl(impl)); }

DI_MODULE_EXPORT
constexpr auto makeAlias(auto& impl, auto key) { return AliasWithKey(detail::compressImpl(impl), key); }

} // namespace di


#endif // INCLUDE_DI_ALIAS_HPP
