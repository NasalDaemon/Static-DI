module;

#if !DI_IMPORT_STD
#include "di/di.hpp"
#endif

export module di;

#if DI_IMPORT_STD
import std;
#define DI_MODULE_EXPORT export
#include "di/di.hpp"
#else

export using ::operator new;

export namespace di {
    // alias.hpp
    using di::Alias;
    using di::makeAlias;
    // cluster.hpp
    using di::Cluster;
    using di::IsCluster;
    using di::Domain;
    // combine.hpp
    using di::Combine;
    // context_fwd.hpp
    using di::Context;
    using di::IsContext;
    using di::ContextParameterOf;
    using di::ContextOf;
    using di::ContextToNode;
    using di::ContextToNodeState;
    using di::NullContext;
    using di::RootContext;
    using di::IsRootContext;
    using di::InlineContext;
    // context.hpp
    using di::ContextHasTrait;
    using di::ContextHasTraitPred;
    // count.hpp
    using di::nodeCount;
    using di::IsUnary;
    using di::EnsureAny;
    using di::EnsureUnary;
    using di::EnsureNonUnary;
    // detached.hpp
    using di::DetachedInterface;
    using di::IsDetachedInterface;
    using di::DetachedImpl;
    using di::IsDetachedImpl;
    using di::HasDetachedContext;
    // empty_types.hpp
    using di::EmptyTypes;
    // environment.hpp
    using di::Environment;
    using di::EnvironmentOverlay;
    using di::EnvironmentComponent;
    using di::mergeEnv;
    using di::withEnv;
    using di::WithEnv;
    // factory.hpp
    using di::WithFactory;
    using di::withFactory;
    using di::Constructor;
    // graph.hpp
    using di::InlineGraph;
    using di::Graph;
    using di::constructGraph;
    // key.hpp
    namespace key {
        using key::Default;
        using key::IsKey;
        using key::Trait;
    }
    // link.hpp
    using di::CanGetNode;
    using di::HasTrait;
    using di::ResolvedLink;
    using di::IsResolvedLink;
    // map_info.hpp
    using di::IsInfoMapper;
    using di::MapInfo;
    // mock.hpp
    namespace test {
        using test::Mock;
    }
    // narrow.hpp
    using di::Narrow;
    // node_fwd.hpp
    using di::Node;
    using di::IsNode;
    using di::IsNodeWrapper;
    using di::WrapNode;
    using di::IsWrappedImpl;
    using di::IsNodeHandle;
    using di::ToNodeWrapper;
    using di::NodeTypes;
    using di::InlineNode;
    // repeater.hpp
    using di::Repeater;
    using di::RepeaterTrait;
    // resolve.hpp
    using di::ResolveTypes;
    using di::ResolveRoot;
    using di::ResolveInfo;
    // test.hpp
    namespace test {
        using test::IsTestContext;
        using test::Local;
        using test::local;
        using test::MockTrait;
        using test::MockKey;
        using test::Cluster;
        using test::Graph;
    }
    // thread.hpp
    using di::ThreadEnvironment;
    using di::OnThread;
    using di::AnyThread;
    using di::withThread;
    namespace key {
        using key::ThreadPost;
    }
    // trait_view.hpp
    using di::IsTraitView;
    using di::IsTraitViewOf;
    using di::AsFunctor;
    using di::asFunctor;
    using di::TraitMethodFunctor;
    using di::AutoCompleteTraitView;
    using di::TraitView;
    using di::makeTraitView;
    // trait.hpp
    using di::AdlTag;
    using di::Trait;
    using di::IsTrait;
    using di::UnconstrainedTrait;
    using di::TraitExpects;
    using di::TraitCanProvide;
    using di::MatchesTrait;
    using di::ExactlyMatchesTrait;
    using di::Implements;
    using di::IsMethodOf;
    using di::TraitOf;
    using di::JoinedTrait;
    using di::AltTrait;
    // traits_fwd.hpp
    using di::ResolvedTrait;
    using di::IsResolvedTrait;
    using di::Traits;
    using di::TraitsOpen;
    using di::TraitsTemplate;
    // union.hpp
    using di::Union;
    using di::withIndex;
    // virtual.hpp
    using di::IDestructible;
    using di::KeepAlive;
    using di::IsInterface;
    using di::IsVirtualContext;
    using di::INode;
    using di::Virtual;
}

#endif
