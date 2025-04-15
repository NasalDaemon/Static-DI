#ifndef INCLUDE_DI_MOCK_HPP
#define INCLUDE_DI_MOCK_HPP

#include "di/detail/type_name.hpp"
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
#include <vector>
#endif

namespace di::test {

DI_MODULE_EXPORT
struct Mock
{
    using ArgTypes = std::vector<std::type_index>;

    struct Return
    {
        template<class T>
        operator T()
        {
            if (not value.has_value() and returnDefault)
                return T();
            if (auto* p = std::any_cast<T>(&value))
                return static_cast<T>(*p);
            throw std::bad_any_cast();
        }
        std::any value;
        bool returnDefault = false;
    };

    using Def = std::function<void(Return& result, void** args)>;
    struct Defs
    {
        Def con, mut;
    };

    enum DefaultBehaviour
    {
        ReturnDefault,
        ThrowIfMissing,
    };

    template<class Context>
    struct Node : di::Node
    {
        using Traits = di::TraitsOpen<Node>;

        Node() = default;

        void reset()
        {
            *this = {};
        }

        void setReturnDefault() { defaultBehaviour = ReturnDefault; }
        void setThrowIfMissing() { defaultBehaviour = ThrowIfMissing; }

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
        Return apply(this Self& self, Tag, Args&&... args)
        {
            ArgTypes argTypes{
                std::type_index{typeid(Tag)},
                std::type_index{typeid(Args)}...};

            self.methodCountMap[std::type_index{typeid(Tag)}]++;
            self.definitionCountMap[argTypes]++;

            Def* impl = nullptr;
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

            Return result;
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
                case ReturnDefault:
                    result.returnDefault = true;
                    break;
                case ThrowIfMissing:
                    {
                    std::string error = "Mock implementation not defined for apply(";
                    error += detail::typeName<Tag>();
                    ((error += ", ", error += detail::typeName<Args>()), ...);
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
            auto& def = isConst ? defs.con : defs.mut;
            def =
                [f = std::forward<F>(f)](Return& result, void** args)
                {
                    auto invoke = [&]<std::size_t... I>(std::index_sequence<I...>)
                    {
                        return std::invoke(f, Tag{}, (Args&&)(*(std::remove_cvref_t<Args>*)(args[I]))...);
                    };
                    if constexpr (std::is_void_v<R>)
                        invoke(std::index_sequence_for<Args...>{});
                    else
                        result.value.template emplace<R>(invoke(std::index_sequence_for<Args...>{}));
                };
        }

        DefaultBehaviour defaultBehaviour = DefaultBehaviour::ReturnDefault;
        mutable std::map<ArgTypes, Defs> definitions;
        mutable std::map<ArgTypes, std::size_t> definitionCountMap;
        mutable std::map<std::type_index, std::size_t> methodCountMap;
    };
};

}

#endif // INCLUDE_DI_MOCK_HPP
