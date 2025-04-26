module;
#if !DI_STD_MODULE
#include <cstddef>
#endif
export module di.tests.thread.poster;

import di;
#if DI_STD_MODULE
import std;
#endif

namespace di::tests::thread {
    struct Poster
    {
        template<class Task>
        static constexpr decltype(auto) post(Task task)
        {
            return task();
        }
    };
    template<std::size_t, std::size_t>
    using PosterAlias = Poster;

    export inline constexpr di::key::ThreadPost<PosterAlias> postTaskKey{};
}
