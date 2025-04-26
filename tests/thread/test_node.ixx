module;
#if !DI_IMPORT_STD
#include <type_traits>
#endif
export module di.tests.thread.test_node;

import di.tests.thread.poster;
import di.tests.thread.traits;
import di;
#if DI_IMPORT_STD
import std;
#endif

export namespace di::tests::thread {

template<class Trait>
struct TestNode : di::Node
{
    struct Interface : di::DetachedInterface
    {
        int apply(this auto const& self, trait::Trait::getA)
        {
            if constexpr (std::is_same_v<Trait, trait::A>)
                return self->i;
            else
                return self.getNode(trait::a).getA();
        }
        int apply(this auto const& self, trait::Trait::getB method)
        {
            if constexpr (std::is_same_v<Trait, trait::B>)
                return self->i;
            else
                return self.getNode(trait::b, postTaskKey).apply(method);
        }
        int apply(this auto const& self, trait::Trait::getC)
        {
            if constexpr (std::is_same_v<Trait, trait::C>)
                return self->i;
            else
                return self.getNode(trait::c).getC();
        }
    };

    using Traits = di::Traits<TestNode(Interface), trait::A, trait::B, trait::C>;

    TestNode(int i) : i(i) {}

    int i;
};

} // namespace di::tests::thread
