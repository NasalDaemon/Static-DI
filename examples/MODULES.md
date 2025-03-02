# Modules example

Example called `my_app` which shows a simple user session manager and authentication server interacting to create user tokens (such as JWTs). It is made of two nodes (`Session` and `AuthService`) in a cluster, which express their dependencies on each other via connections in the cluster. The connections are named by the trait that satisfies the dependency.

The nodes also have two dependencies on data types:
1. `Session` derives its `PassHash` type from the one provided at the root of the graph via `Context::Root`.
2. `Session` derives its `Token` type from the one provided by `AuthService` via `di::ResolveTypes` static trait resolution.

<details>
<summary>:warning: NOTE: the data type dependencies are simply showing off features! Don't resolve data types inside nodes just because you can!</summary>

> It is generally recommended for simplicity that well-known data types used by multiple nodes are defined _independently_ of and _externally_ to nodes. Humans tend to find types much more easily through a well organised file structure, rather than traversing clusters of interdependent nodes. If a data type does need to be configurable using `Context::Root` or `ResolveTypes`, the concrete type itself still should be indepedently defined in a well-named class that is easy to find. It can then be simply aliased in the custom root or the node's trait type.
>
> Order of preference for dependencies on data types:
> 1. Type defined externally and used directly by nodes
>    - No type resolution keeps things explicit
>    - Easier to define non-template member functions, which can easily be split into a separate .cpp for faster compilation
> 2. Type defined externally and aliased in a custom graph root
>    - Type alias is easy to find without walking any nodes of the graph
> 3. Type defined externally and aliased in node trait types _if number of affected nodes and traits is small_
>    - Type alias is not too distant from the current node
> 4. Type defined internally to a mock node for testing purposes and aliased in the trait types
>    - Mocks are defined in the same source file as the tests

</details>

## File structure
- [my/CMakeLists.txt](#cmakeliststxt)
- [my/traits.ixx.dig](#traitsixxdig): `my::trait::AuthService`, `my::trait::TokenStore`, `my::trait::SessionManager`: traits for nodes to implement
- [my/cluster.ixx.dig](#clusterixxdig): `my::Cluster`: A cluster of interconnected nodes implementing the traits
- [my/token.ixx](#tokenixx): `my::Token` data type stores the proof of identity for a user, issued by the AuthService with an expiry
- [my/pass_hash.ixx](#pass_hashixx): `my::PassHash` data type stores the hash of passwords
- [my/auth_service.ixx](#auth_serviceixx): `my::AuthService` node implements the trait `my::trait::AuthService`
- [my/sessions.ixx](#sessionsixx): `my::Sessions` node implements the traits `my::trait::TokenStore` and `my::trait::SessionManager`
- [my/main.cpp](#maincpp): Constructs and uses the full graph of nodes which satisfies all requirements of the nodes within `my::Cluster`
- my/auth_service.cpp (not shown)
- my/db.ixx (not shown)
- my/task.ixx (not shown)

## CMakeLists.txt
```CMake
add_executable(my_app main.cpp)

target_link_libraries(my_app PUBLIC di::module)

# Add module interface units and implementation units
target_sources(my_app
    PUBLIC FILE_SET CXX_MODULES FILES
        token.ixx
        pass_hash.ixx
        auth_service.ixx
        sessions.ixx
        db.ixx # not shown
        task.ixx # not shown
    PRIVATE
        auth_service.cpp # not shown (implements AuthService::dbValidPass etc)
        token.cpp # not shown
        pass_hash.cpp # not shown
)

# Adds traits.ixx (from traits.ixx.dig) and cluster.ixx (from cluster.ixx.dig)
# by searching recursively for *.ixx.dig in the current directory
target_generate_di_modules(my_app)
```

## traits.ixx.dig
```
export module my.traits;

import std;
import my.task; // not shown in example

namespace my::trait {

trait AuthService [Types]
{
    // Ensures Token type can be resolved by clients of this trait
    requires typename Types::Token

    // Check user/pass against database, and return if combo is valid
    logIn(std::string_view user, std::string_view pass) -> Task<bool>

    // Check user/oldPass against database, and return if password was changed
    changePass(std::string_view user, std::string_view oldPass, std::string_view newPass) -> Task<bool>
}

trait TokenStore [Types, Root]
{
    // Ensures PassHash is defined at the root of the graph when using this trait
    requires typename Root::PassHash
    requires typename Types::Token

    // Store latest token generated for a user
    store(std::string_view user, Root::PassHash passHash, Types::Token token)

    // Delete any token stored against this user
    revoke(std::string_view user)
}

trait SessionManager [Types]
{
    requires typename Types::Token

    // Has a token been created for user that has not expired
    hasToken(std::string_view user) const -> bool

    // Get token for user using the password if necessary
    getToken(std::string_view user, std::string_view pass) -> Task<std::optional<typename Types::Token>>
}

}
```

## cluster.ixx.dig
`my::Cluster`: A cluster of interconnected nodes implementing the traits
```
export module my.cluster;

import my.sessions;
import my.auth_service;
import my.traits;

namespace my {

cluster Cluster
{
    sessions = Sessions
    authService = AuthService

    [trait::TokenStore <-> trait::AuthService]
    sessions <-> authService

    // Above is equivalent to:
    // [trait::TokenStore]  sessions <-- authService
    // [trait::AuthService] sessions --> authService
}

}
```
## token.ixx
`my::Token` data type stores the proof of identity for a user, issued by the AuthService with an expiry
```cpp
export module my.token;

namespace my {
    export struct Token
    {
        bool expired() const;
    private:
        // ... some data fields ...
    };
}
```
## pass_hash.ixx
`my::PassHash` data type stores the hash of passwords
```cpp
export module my.pass_hash;

namespace my {
    export struct PassHash
    {
        explicit PassHash(std::string_view s);
        friend constexpr bool sameHash(PassHash const& self, std::string_view pass);
    private:
        // ... some data fields ...
    };
}
```
## auth_service.ixx
`my::AuthService` node implements the trait `my::trait::AuthService`
```cpp
export module my.auth_service;

import my.token;
import my.traits;
import my.db; // not shown in example
import my.task; // not shown in example

import di;
import std;

namespace my {

// Note: non-template functions in non-template class can be easily implemented
// in separate .cpp files for faster compilation.
export struct AuthService : di::Node
{
    using Traits = di::Traits<AuthService, trait::AuthService>;
    // shorthand for:
    //      di::Traits<AuthService
    //          , trait::AuthService(AuthService, AuthService::Types)
    //      >

    struct Types
    {
        // exposes Token type to client nodes, as is required by `trait::AuthService`
        using Token = my::Token;
    };

    // Trait methods are to be implemented with the signature: apply(<Trait>::<method>, args...)
    template<class Self>
    Task<bool> apply(
        this Self& self, // deducing-this deduces the node's context in the graph
        trait::AuthService::logIn,
        // take user+pass as owning strings as this is a coroutine (still meets trait requirement)
        std::string user,
        std::string pass)
    {
        self.dbQueryCount++;
        bool const success = co_await self.dbValidPass(user, pass);
        if (success)
        {
            // Resolves to `my::PassHash` in this example
            using PassHash = di::ContextOf<Self>::Root::PassHash;
            // Since the type and memory offset of `Sessions` with respect to `AuthService` is known
            // statically via the context of `Self`, this entire call can/will be inlined by the compiler:
            self.getNode(trait::tokenStore).store(user, PassHash(pass), self.makeToken(user));
            // In this example project, this is effectively equivalent to:
            // <my::Cluster>.sessions.asTrait(trait::tokenStore).store(user, PassHash(...), makeToken(...));
            // which directly calls
            // <my::Cluster>.sessions.apply(trait::TokenStore::store{}, user, PassHash(...), makeToken(...));
        }
        co_return success;
    }

    Task<bool> apply(
        this auto& self,
        trait::AuthService::changePass,
        std::string user,
        std::string oldPass,
        std::string newPass)
    {
        self.dbQueryCount++;
        bool const success = co_await self.dbChangePass(user, oldPass, newPass);
        if (success)
            self.getNode(trait::tokenStore).revoke(user);
        co_return success;
    }

    // Queries database asynchronously and returns `true` in calling thread if the pass is valid.
    Task<bool> dbValidPass(std::string_view user, std::string_view pass) const;
    Task<bool> dbChangePass(std::string_view user, std::string_view oldPass, std::string_view newPass) const;
    Token makeToken(std::string_view user);

    AuthService(std::string tokenSecret, std::chrono::seconds expiry)
        : tokenSecret(std::move(tokenSecret))
        , expiry(expiry)
        , db(makeDbHandle(...))
    {}

    std::size_t dbQueryCount = 0;
    std::string tokenSecret;
    std::chrono::seconds expiry;
    DatabaseHandle db;
};

}
```
## sessions.ixx
`my::Sessions` node implements the traits `my::trait::TokenStore` and `my::trait::SessionManager`
```cpp
export module my.sessions;

import my.traits;

import di;
import std;

namespace my {

export struct Sessions
{
    // If implementing a node with data type dependencies, a nested
    // Node<Context> template class is needed to query the types in the graph
    template<class Context>
    struct Node : di::Node
    {
        using Traits = di::Traits<Node, trait::TokenStore, trait::SessionManager>;
        // shorthand for:
        //      di::Traits<Node
        //          , trait::TokenStore(Node, Node::Types)
        //          , trait::SessionManager(Node, Node::Types)
        //      >

        struct Types
        {
            // Exposes Token type to satisfy `trait::TokenStore` and `trait::SessionManager`
            // by deferring to the type provided by `trait::AuthService`
            using Token = di::ResolveTypes<Context, trait::AuthService>::Token;
            // Resolves to `my::AuthService::Types::Token` in this example
        };

        using Token = Types::Token;

        // Resolves to `my::PassHash` in this example
        using PassHash = Context::Root::PassHash;

        void apply(trait::TokenStore::store, std::string_view user, PassHash passHash, Token token)
        {
            auto const [it, inserted] = userDetailsMap.try_emplace(user, std::move(token), std::move(passHash));
            if (not inserted)
            {
                it->token = std::move(token);
                it->passHash = std::move(passHash);
            }
        }

        void apply(trait::TokenStore::revoke, std::string_view user)
        {
            userDetailsMap.erase(user);
        }

        bool apply(trait::SessionManager::hasToken, std::string_view user) const
        {
            if (auto const userDetails = getUserDetails(user))
                if (not userDetails->token.expired())
                    return true;
            return false;
        }

        Task<std::optional<Token>> apply(
            trait::SessionManager::getToken,
            std::string_view user,
            std::string_view pass)
        {
            // To avoid db query: return cached token if one exists that has not expired
            if (auto const userDetails = getUserDetails(user))
                if (userDetails->validWithPass(pass))
                    co_return userDetails->token;

            // Else, try to log in and return token if successful
            // Note: `getNode` can be called directly, since `Context` is immediately available
            if (co_await getNode(trait::authService).logIn(user, pass))
                co_return getUserDetails(user)->token;

            co_return std::nullopt;
        }

        auto const* getUserDetails(std::string_view user) const
        {
            auto const tokenIt = userDetailsMap.find(user);
            return tokenIt != userDetailsMap.end()
                ? std::addressof(tokenIt->second)
                : nullptr;
        }

        Node() = default;

        struct UserDetails
        {
            UserDetails(Token token, PassHash passHash)
                : token(std::move(token))
                , passHash(std::move(passHash))
            {}

            Token token;
            PassHash passHash; // avoid storing passwords in plaintext

            bool validWithPass(std::string_view pass) const
            {
                return not token.expired() and sameHash(passHash, pass);
            }
        };

        std::unordered_map<std::string, UserDetails> userDetailsMap;
    };
};

}
```

## main.cpp
Constructs the full graph of nodes which satisfies all requirements of the nodes within `my::Cluster`
```cpp
import my.cluster;
import my.pass_hash;
import my.traits;
import di;

using namespace my;

int main()
{
    struct Root
    {
        using PassHash = my::PassHash
    };

    di::Graph<my::Cluster, Root> graph{
        .authService{"super token secret", std::chrono::seconds{600}}
    };
    // `di::Graph<my::Cluster, Root>` is an alias to `my::Cluster<di::RootContext<Root>>`, which is roughly:
    //  struct my::PsuedoGeneratedCluster
    //  {
    //      struct SessionsContext { ... };
    //      struct AuthServiceContext { ... };
    //
    //      using SessionsNode    = Sessions::template Node<SessionsContext>;
    //      using AuthServiceNode = di::WrapNode<AuthService>::template Node<AuthServiceContext>;
    //
    //      SessionsNode    sessions{};
    //      AuthServiceNode authService{};
    //
    //      void visit(auto const& visitor)
    //      {
    //          sessions.visit(visitor);
    //          authService.visit(visitor);
    //      }
    //  };

    auto sessionManager = graph.sessions.asTrait(trait::sessionManager);
    assert(not sessionManager.hasToken("user1")); // user1 not logged in yet
    // same as sessionManager.apply(trait::SessionManager::hasToken{}, "user1")
    // which directly calls graph.sessions.apply(trait::SessionManager::hasToken{}, "user1")

    // Note, the following do not compile (inaccessible) as they are not part of `trait::SessionManager`:
    // sessionManager.userDetails;
    // sessionManager.getUserDetails("user1");
    // sessionManager.getNode(trait::authService);
    // sessionManager.apply(trait::TokenStore::store, "user1", Token{});
    // sessionManager.store("user1", Token{});
    // This ensures that the details of a node cannot leak from a trait, keeping the interface robust,
    // and absolves nodes of the responsibility to hide away their state and implementation details,
    // since other nodes in the graph are only able to access their traits via getNode(<trait>).

    auto authService = graph.authService.asTrait(trait::authService);
    auto user1LoginTask = authService.logIn("user1", "user1Pass");
    if (user1LoginTask.value()) // synchronously waits for task to complete (as we are in main)
    {
        // assuming the token hasn't expired yet
        assert(sessionManager.hasToken("user1"));
        assert(sessionManager.getToken("user1", "user1Pass").value().has_value());
        // change password should be successful
        assert(authService.changePass("user1", "user1Pass", "newUser1Pass").value());
        // token has been revoked
        assert(not sessionManager.hasToken("user1"))
        // can get token with new password
        assert(sessionManager.getToken("user1", "newUser1Pass").value().has_value());
    }
}
```
