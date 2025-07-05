#pragma once

#include "abc/traits.hpp"

#include "di/depends.hpp"
#include "di/node.hpp"
#include "di/resolve.hpp"
#include "di/traits.hpp"

namespace abc {

struct EllieType{};
struct EllieType3{};

struct Ellie : di::Node
{
    using Depends = di::Depends<trait::Charlie>;

    struct Charlie;
    struct Charlie2;
    struct Ellie3Types;

    using Traits = di::Traits<Ellie
        , trait::Ellie
        , trait::Ellie3*(Ellie3Types)
        , trait::Charlie(Charlie)
        , trait::Charlie2(Charlie2)
    >;

    // Degenerate case, but just to prove that it works...
    struct Charlie2 : di::DetachedInterface
    {
        template<class Self>
        int apply(this Self const& self, trait::Charlie::get)
        {
            using CharlieType = di::ResolveTypes<Self, trait::Charlie>::CharlieType;
            static_assert(std::is_same_v<int, CharlieType>);
            return self.getNode(trait::charlie).get();
        }
    };
    struct Ellie3Types
    {
        using EllieType = abc::EllieType3;
    };

    struct Types
    {
        using CharlieType = int;
        using EllieType = abc::EllieType;
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
        using CharlieType = di::ResolveTypes<Self, trait::Charlie>::CharlieType;
        static_assert(std::is_same_v<int, CharlieType>);
        return self.getNode(trait::charlie).get();
    }
};

}
