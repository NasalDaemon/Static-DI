module;

#if !DI_STD_MODULE
#include "di/di.hpp"
#include "di/test.hpp"
#include "di/thread.hpp"
#include "di/union.hpp"
#include "di/virtual.hpp"
#endif

export module di;

#if DI_STD_MODULE
import std;
#define DI_MODULE_EXPORT export
#include "di/di.hpp"
#include "di/test.hpp"
#include "di/thread.hpp"
#include "di/union.hpp"
#include "di/virtual.hpp"
#else

export using ::operator new;

export namespace di {
    // di.hpp
    using di::NullContext;
    using di::IsContext;
    using di::RootContext;
    using di::IsRootContext;
    using di::AdlTag;
    using di::Implements;
    using di::Trait;
    using di::TraitExpects;
    using di::IsTrait;
    using di::IsMethodOf;
    using di::TraitOf;
    using di::JoinedTrait;
    using di::AltTrait;
    using di::MatchesTrait;
    using di::ExactlyMatchesTrait;
    using di::CanGetNode;
    using di::HasTrait;
    using di::Alias;
    using di::NodeTypes;
    using di::EmptyTypes;
    using di::TraitView;
    using di::IsTraitView;
    using di::IsTraitViewOf;
    using di::AsFunctor;
    using di::asFunctor;
    using di::TraitMethodFunctor;
    using di::ResolvedLink;
    using di::IsResolvedLink;
    using di::ContextParameterOf;
    using di::ContextOf;
    using di::IsNode;
    using di::IsNodeWrapper;
    using di::WrapNode;
    using di::IsNodeHandle;
    using di::ToNodeWrapper;
    using di::ContextToNode;
    using di::ContextToNodeState;
    using di::Context;
    using di::DetachedInterface;
    using di::IsDetachedInterface;
    using di::Node;
    using di::Cluster;
    using di::InlineNode;
    using di::InlineContext;
    using di::InlineGraph;
    using di::Graph;
    using di::constructGraph;
    using di::ResolvedTrait;
    using di::IsResolvedTrait;
    using di::Traits;
    using di::TraitsOpen;
    using di::TraitsTemplate;
    using di::ResolveTypes;
    using di::ResolveRoot;
    using di::ResolveInfo;
    using di::Repeater;
    using di::RepeaterTrait;

    using di::AutoCompleteTraitView;

    // thread.hpp
    using di::Environment;
    using di::mergeEnv;
    using di::withEnv;
    using di::WithEnv;
    using di::withThread;
    using di::OnThread;
    using di::AnyThread;
    using di::ThreadEnvironment;
    namespace key {
        using key::ThreadPost;
        using key::Default;
    }

    // test.hpp
    namespace test {
        using test::IsTestContext;
        using test::Local;
        using test::local;
        using test::Test;
        using test::Graph;
    }

    // union.hpp
    using di::Union;
    using di::withIndex;

    // virtual.hpp
    using di::IRemotes;
    using di::INode;
    using di::Virtual;
}

#endif
