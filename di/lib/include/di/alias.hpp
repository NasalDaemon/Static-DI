#ifndef INCLUDE_DI_ALIAS_HPP
#define INCLUDE_DI_ALIAS_HPP

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
    constexpr explicit Alias(T& ref) : ref(std::addressof(ref)) {}

    template<class Self>
    constexpr auto& get(this Self&& self) { return std::forward_like<Self&>(*self.ref); }
    constexpr auto& operator*(this auto&& self) { return self.get(); }
    constexpr auto* operator->(this auto&& self) { return std::addressof(self.get()); }

private:
    T* ref;
};

DI_MODULE_EXPORT
template<class T>
Alias(T&) -> Alias<T>;

} // namespace di


#endif // INCLUDE_DI_ALIAS_HPP
