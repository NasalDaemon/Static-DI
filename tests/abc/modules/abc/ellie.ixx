module;
#if !DI_STD_MODULE
#include <type_traits>
#endif
export module abc.ellie;

import abc.traits;
import di;
DI_IMPORT_STD;

export namespace abc {

struct Ellie : di::Node
{
    struct Charlie;
    // Degenerate case, but just to prove that it works...
    struct Charlie2 : di::DetachedInterface
    {
        template<class Self>
        int apply(this Self const& self, trait::Charlie::get)
        {
            using CharlieType = di::ResolveTypes<di::ContextOf<Self>, trait::Charlie>::CharlieType;
            static_assert(std::is_same_v<int, CharlieType>);
            return self.getNode(trait::charlie).get();
        }
    };

    using Traits = di::Traits<Ellie
        , trait::Ellie
        , trait::Charlie(Charlie)
        , trait::Charlie2(Charlie2)
    >;

    struct Types
    {
        using CharlieType = int;
    };

    inline Ellie(int value = 101) : value(value) {}

    void onGraphConstructed();

    int apply(trait::Ellie::get) const;

    int value;
};

struct Ellie::Charlie : Ellie
{
    template<class Self>
    int apply(this Self const& self, trait::Charlie::get)
    {
        using CharlieType = di::ResolveTypes<di::ContextOf<Self>, trait::Charlie>::CharlieType;
        static_assert(std::is_same_v<int, CharlieType>);
        return self.getNode(trait::charlie).get();
    }
};

}
