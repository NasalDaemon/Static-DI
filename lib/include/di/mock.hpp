#ifndef INCLUDE_DI_MOCK_HPP
#define INCLUDE_DI_MOCK_HPP

#include "di/detail/type_name.hpp"
#include "di/empty_types.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/trait.hpp"
#include "di/traits.hpp"

#if !DI_STD_MODULE
#include <any>
#include <functional>
#include <map>
#include <span>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <variant>
#include <vector>
#endif

namespace di::test {

namespace detail {

    struct MockReturn
    {
        template<class T>
        operator T&()
        {
            if (std::any* any = std::get_if<std::any>(&value))
            {
                if (T* p = std::any_cast<T>(any))
                    return *p;
                throw std::bad_any_cast();
            }
            else if (Ref* ref = std::get_if<Ref>(&value))
            {
                if (ref->type != std::type_index{typeid(T&)} or not ref->lref)
                    throw std::bad_any_cast();
                return *static_cast<T*>(ref->ptr);
            }
            else if (returnDefault)
            {
                return value.emplace<std::any>().emplace<T>();
            }
            throw std::bad_any_cast();
        }

        template<class T>
        operator T&&()
        {
            if (std::any* any = std::get_if<std::any>(&value))
            {
                if (T* p = std::any_cast<T>(any))
                    return std::move(*p);
                throw std::bad_any_cast();
            }
            else if (Ref* ref = std::get_if<Ref>(&value))
            {
                if (ref->type != std::type_index{typeid(T&&)} or ref->lref)
                    throw std::bad_any_cast();
                return std::move(*static_cast<T*>(ref->ptr));
            }
            else if (returnDefault)
            {
                return std::move(value.emplace<std::any>().emplace<T>());
            }
            throw std::bad_any_cast();
        }

        template<class T, class Arg>
        void emplace(Arg&& arg)
        {
            if constexpr (std::is_reference_v<T>)
                value.emplace<Ref>(std::addressof(arg), typeid(T), std::is_lvalue_reference_v<T>);
            else
                value.emplace<std::any>(std::in_place_type<T>, std::forward<Arg>(arg));
        }

        void reset() { value.emplace<std::monostate>(); }

        void setReturnDefault() { returnDefault = true; }

    private:
        bool returnDefault = false;

        struct Ref
        {
            void* ptr = nullptr;
            std::type_index type;
            bool lref = false;
        };
        std::variant<std::monostate, std::any, Ref> value;
    };

    using MockDef = std::function<void(MockReturn& result, void** args)>;
    struct MockDefs
    {
        MockDef con, mut;
    };

    enum MockDefaultBehaviour
    {
        ReturnDefault,
        ThrowIfMissing,
    };

} // namespace detail

DI_MODULE_EXPORT
template<class DefaultTypes = EmptyTypes, class... MockedTraits>
struct Mock
{
    using ArgTypes = std::vector<std::type_index>;

    template<class Context>
    struct Node : di::Node
    {
        using Traits = std::conditional_t<
            sizeof...(MockedTraits) == 0,
            di::TraitsOpen<Node>,
            di::Traits<Node, MockedTraits...>
        >;

        Node() = default;

        using Types = DefaultTypes;

        void reset()
        {
            *this = {};
        }

        void setReturnDefault() { defaultBehaviour = detail::MockDefaultBehaviour::ReturnDefault; }
        void setThrowIfMissing() { defaultBehaviour = detail::MockDefaultBehaviour::ThrowIfMissing; }

        template<class Tag>
        std::size_t methodCallCount(Tag) const
        {
            auto const it = methodCountMap.find(std::type_index{typeid(Tag)});
            return it != methodCountMap.end() ? it->second : 0ul;
        }
        template<class Tag, class... Args>
        std::size_t definitionCallCount() const
        {
            ArgTypes argTypes{
                std::type_index{typeid(Tag)},
                std::type_index{typeid(Args)}...};

            auto const it = definitionCountMap.find(argTypes);
            return it != definitionCountMap.end() ? it->second : 0ul;
        }

        template<class Self, class Tag, class... Args>
        detail::MockReturn apply(this Self& self, Tag, Args&&... args)
        {
            ArgTypes argTypes{
                std::type_index{typeid(Tag)},
                std::type_index{typeid(Args)}...};

            self.methodCountMap[std::type_index{typeid(Tag)}]++;
            self.definitionCountMap[argTypes]++;

            detail::MockDef* impl = nullptr;
            if (auto const it = self.definitions.find(argTypes); it != self.definitions.end())
            {
                if constexpr (std::is_const_v<Self>)
                {
                    if (it->second.con)
                        impl = &it->second.con;
                }
                else
                {
                    if (it->second.mut)
                        impl = &it->second.mut;
                    else if (it->second.con)
                        impl = &it->second.con;
                }
            }

            detail::MockReturn result;
            if (impl != nullptr)
            {
                if constexpr (sizeof...(args) > 0)
                {
                    void* a[] = {std::addressof(args)...};
                    std::invoke(*impl, result, a);
                }
                else
                {
                    std::invoke(*impl, result, nullptr);
                }
            }
            else
            {
                switch (self.defaultBehaviour)
                {
                case detail::MockDefaultBehaviour::ReturnDefault:
                    result.setReturnDefault();
                    break;
                case detail::MockDefaultBehaviour::ThrowIfMissing:
                    {
                    std::string error = "Mock implementation not defined for apply(";
                    error += di::detail::typeName<Tag>();
                    ((error += ", ", error += di::detail::typeName<Args>()), ...);
                    if constexpr (std::is_const_v<Self>)
                        error += ") const";
                    else
                        error += ')';
                    throw std::runtime_error(error);
                    }
                }
            }

            return result;
        }

        template<class... Fs>
        void define(Fs... fs)
        {
            (defineImpl((decltype(getTypes(&Fs::operator()))) nullptr, false, fs), ...);
            (defineImpl((decltype(getTypes(&Fs::operator()))) nullptr, true, fs), ...);
        }
        template<class... Fs>
        void defineConst(Fs... fs)
        {
            (defineImpl((decltype(getTypes(&Fs::operator()))) nullptr, true, std::move(fs)), ...);
        }
        template<class... Fs>
        void defineMut(Fs... fs)
        {
            (defineImpl((decltype(getTypes(&Fs::operator()))) nullptr, false, std::move(fs)), ...);
        }

    private:
        template<class R, class F, class... Args>
        static auto getTypes(R (F::*)(Args...)) -> R(*)(Args...);
        template<class R, class F, class... Args>
        static auto getTypes(R (F::*)(Args...) const) -> R(*)(Args...);

        template<class R, class Tag, class... Args, class F>
        void defineImpl(R(*)(Tag, Args...), bool isConst, F&& f)
        {
            ArgTypes argTypes{
                std::type_index{typeid(std::remove_cvref_t<Tag>)},
                std::type_index{typeid(Args)}...};
            auto& defs = definitions[argTypes];
            (isConst ? defs.con : defs.mut) =
                [f = std::forward<F>(f)](this auto&, detail::MockReturn& result, void** args)
                {
                    [&]<std::size_t... I>(std::index_sequence<I...>) -> decltype(auto)
                    {
                        if constexpr (std::is_void_v<R>)
                        {
                            result.reset();
                            std::invoke(f, Tag{}, static_cast<Args&&>(*static_cast<std::remove_cvref_t<Args>*>(args[I]))...);
                        }
                        else
                        {
                            result.emplace<R>(
                                std::invoke(f, Tag{}, static_cast<Args&&>(*static_cast<std::remove_cvref_t<Args>*>(args[I]))...));
                        }
                    }(std::index_sequence_for<Args...>{});
                };
        }

        detail::MockDefaultBehaviour defaultBehaviour = detail::MockDefaultBehaviour::ReturnDefault;
        mutable std::map<ArgTypes, detail::MockDefs> definitions;
        mutable std::map<ArgTypes, std::size_t> definitionCountMap;
        mutable std::map<std::type_index, std::size_t> methodCountMap;
    };
};

}

#endif // INCLUDE_DI_MOCK_HPP
