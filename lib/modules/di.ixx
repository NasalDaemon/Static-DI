module;

#if !DI_IMPORT_STD
#include "di/di.hpp"
#endif

export module di;

#if DI_IMPORT_STD
export import std;
#define DI_MODULE_EXPORT export
#include "di/di.hpp"
#else

export using ::operator new;

export namespace di {
    // adapt.hpp
    using di::Adapt;
    using di::adapt;
    // alias.hpp
    using di::Alias;
    using di::makeAlias;
    // args.hpp
    using di::Args;
    using di::args;
    using di::IsArgs;
    using di::IsArgsOf;
    // box.hpp
    using di::BoxWithRoot;
    using di::boxWithRoot;
    using di::Box;
    using di::box;
    // cluster.hpp
    using di::Cluster;
    using di::IsCluster;
    using di::Domain;
    using di::DomainParams;
    // collection.hpp
    namespace key {
        using key::Element;
        using key::Elements;
        using key::allElements;
    }
    using di::Collection;
    // combine.hpp
    using di::Combine;
    // compiler.hpp
    using di::Version;
    using di::Compiler;
    using di::isClang;
    using di::isGcc;
    using di::isGnu;
    using di::isMsvc;
    using di::compiler;
    using di::clang;
    using di::gcc;
    using di::msvc;
    // context_fwd.hpp
    using di::Context;
    using di::IsContext;
    using di::IsCollectionContext;
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
    // depends.hpp
    using di::Depends;
    // detached.hpp
    using di::DetachedInterface;
    using di::IsDetachedInterface;
    using di::DetachedImpl;
    using di::IsDetachedImpl;
    using di::HasDetachedContext;
    // defer.hpp
    using di::Defer;
    // empty_types.hpp
    using di::EmptyTypes;
    // ensure.hpp
    using di::Ensure;
    namespace pred {
        using pred::Unary;
        using pred::NonUnary;
        using pred::Stateless;
        using pred::HasDepends;
    }
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
    using di::Emplace;
    // function.hpp
    using di::Function;
    using di::FunctionPolicy;
    // global_context.hpp
    using di::ContextHasGlobal;
    using di::ContextHasGlobalTrait;
    // global_graph.hpp
    using di::GraphWithGlobal;
    // global_trait.hpp
    using di::Global;
    using di::IsGlobalTrait;
    using di::IsNonGlobalTrait;
    using di::global;
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
    // lazy.hpp
    using di::Lazy;
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
    using di::InlineNode;
    using di::NodeHasDepends;
    using di::NodeDependencyAllowed;
    using di::NodeDependenciesSatisfied;
    // no_trait.hpp
    using di::NoTrait;
    using di::noTrait;
    using di::NullTrait;
    using di::IsNoTrait;
    using di::NoTraits;
    // peer_node.hpp
    using di::PeerNode;
    using di::PeerDetached;
    using di::PeerDetachedOpen;
    // peer.hxx
    namespace trait {
        using trait::Peer;
    }
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
        using test::TestOnlyNode;
        using test::Cluster;
        using test::Graph;
        using test::GraphWithGlobal;
    }
    // thread.hpp
    using di::Thread;
    using di::ThreadEnvironment;
    using di::OnThread;
    using di::AnyThread;
    using di::withThread;
    using di::OnDynThread;
    namespace key {
        using key::ThreadPost;
        using key::DynThreadAssert;
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
