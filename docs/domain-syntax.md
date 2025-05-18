# Domains

### PLEASE NOTE:
Domains in Static-DI are special `cluster`s with extra restrictions. If you are unfamiliar with `cluster` syntax in Static-DI, please first familiarise yourself with `cluster`s and their syntax [here](cluster-syntax.md).

## Principal motivation

When a project grows in size, the structure of the dependency graph itself increasingly has an impact on the maintainability and simplicity of the codebase. Dependency graphs often become more complex when there are longer chains of dependencies, cyclic dependencies, and deeper nesting of clusters. Greater complexity often makes it more difficult to add new functionality, maintain the existing functionality, and overall reason about and explain the behaviour of the code.

A `domain` in Static-DI is a special kind of `cluster` which when utilised throughout the entire graph encourages a flatter and shallower graph shape overall with looser coupling between nodes, and encourages thoughtful design in terms of delegation of responsibility into logical domains. This is acheived by preferring certain kinds of dependencies and by making explicit the statefulness and arity of sub-nodes (i.e. whether a sub-node is unary or a cluster/domain).

NOTE: `domain`s will not solve all technical problems. Astute architectural decisions will result in various patterns, and appropriately using those encouraged by `domain` will generally result in a simpler composition. The intention behind `domain` is to highlight particular dependency anti-patterns to prevent them from pervading the codebase, but there may be situations when the less restrictive `cluster` is preferable. In other words: there are exceptions that prove the rule.

### Scale-free networks
In graph theory, the [scale-free](https://en.wikipedia.org/wiki/Scale-free_network) topology describes graphs with a degree distribution (distribution of number of connections per node) that follows a power-law. Below is a visual example representing a scale-free topology:

[<img src="https://github.com/user-attachments/assets/7ba75f9c-02da-49ce-91a3-89be1ed98651" style="background-color:lightblue;" />](https://www.researchgate.net/publication/263268796_A_complex_network_approach_to_supply_chain_network_theory)

[Scale-free clustering (wikipedia):](https://en.wikipedia.org/wiki/Scale-free_network#Clustering):
> Another important characteristic of scale-free networks is the clustering coefficient distribution, which decreases as the node degree increases. This distribution also follows a power law. This implies that the low-degree nodes belong to very dense sub-graphs and those sub-graphs are connected to each other through hubs. Consider a social network in which nodes are people and links are acquaintance relationships between people. It is easy to see that people tend to form communities, i.e., small groups in which everyone knows everyone (one can think of such community as a complete graph). In addition, the members of a community also have a few acquaintance relationships to people outside that community. Some people, however, are connected to a large number of communities (e.g., celebrities, politicians). Those people may be considered the hubs responsible for the small-world phenomenon.

Complex networks tend to evolve towards a highly resilient scale-free topology to adapt to sporadic change and growth. For example: the shape of the internet is approximatly scale-free as online services that are resilient enough to meet the increasing demand of their clients organically become significant hubs. Ideally, codebases should likewise remain robust in the face of inevitable change and growth of scope, where bugs are rarely introduced as code is added or refactored.

The fact that social networks often form approximately scale-free networks also suggests that it is a suitable topology to aspire to in the dependency graph of a large project developed by multiple teams. One can conceive of the responsibility of `clusters` and `domain`s mapping fairly neatly onto the responsibility of teams, or vice-versa, and the dependencies between teams would then also be expressed explicitly in the Static-DI DIG. By clarifying coupling between teams, cross-team dependencies can optimised to improve the overall velocity in the project as a whole, without compromising its resiliency.

## Emergence of good topology

According to various studies in the literature, scale-free networks emerge on iteratively growing networks when new nodes preferentially connect to nodes with a higher degree (i.e. hubs connecting to many other nodes). Static-DI `domains` aim to encourage the emergence of approximately scale-free graphs through the following:

* Every `domain` must have a _single_ unary nexus node which acts as the central hub, or coordinator for the `domain`
* Sub-`cluster`s or sub-`domain`s must have names in ALL_CAPS
* Unary nodes (including the nexus) in the `domain` that have state must have names starting with a capital letter (e.g. CamelCase), and stateless nodes must have names starting with lowercase letter (e.g. pascalCase)
* Only the nexus node is allowed any outward-facing connection to the parent `..` of the `domain`
* Unary sub-nodes may only depend on the nexus node, and not connect to any other nodes
  * Only the nexus node may depend on unary sub-nodes, and unary sub-nodes may only depend on the nexus node
  * Any unary-to-unary connection can be explicitly enabled with strong arrows (`--->>>`) which get progressibely uglier (`---->>>>`) as the total number of overrides in the domain increases to discourage this type of connection
  * This encourages tightly-coupled unary nodes to be either merged into one node, or put into a sub-`domain`
* Sub-`clusters` and sub-`domains` may connect directly to each other
  * sub-`domain`-to-sub-`domain` connections are effectively nexus-to-nexus connections hosted in the greater `domain`

# Domain syntax

The domain syntax is identical to the cluster syntax, with a few key differences:

* The `domain` keyword is used instead of `cluster` to introduce a `domain` block
* The first node named is the nexus node, which will be made to conform to the responsibilities of the nexus
* Node names must be spelt correctly to represent the following properties, which are enforced in the C++ build step once all the types are known:
    * Stateless unary: pascalCase
    * Stateful unary: CamelCase
    * Non-unary (sub-domain or sub-cluster): ALL_CAPS
* unary-to-unary sub-node connections must be explicitly enabled with strong arrows `--->>>`, all of which will be made to get stronger as the total number of these connections increases

## Example
```
domain ShopDomain
{
    api = ShopRestAPI // `api` is nexus
    CUSTOMER = CustomerDomain // sub-domain
    ORDER = OrderDomain // sub-domain
    DELIVERY = DeliveryDomain // sub-domain

    using req = trait::ShopRequest,         res = trait::ShopResponse
    using custReq = trait::CustomerRequest, custRes = trait::CustomerResponse
    using orderReq = trait::OrderRequest,   orderRes = trait::OrderResponse
    using delReq = trait::DeliveryRequest,  delRes = trait::DeliveryResponse

    [res      <-> req]      .. <-> api
    [custRes  <-> custReq]         api <-> CUSTOMER
    [orderRes <-> orderReq]        api <-> ORDER
    [delRes   <-> delReq]          api <-> DELIVERY

    // Order may be cancelled due to customer request, compliance or delivery problems
    [trait::OrderCancel]
    api, CUSTOMER, DELIVERY --> ORDER
}

domain CustomerDomain
{
    gw = CustomerGateway // gw is nexus
    Accounts = CustomerAccounts
    Regs = CustomerRegulations

    using res = trait::CustomerResponse, req = trait::CustomerRequest
    [res <-> req]
    .. <-> gw

    [trait::OrderCancel]
    .. <-- gw

    [trait::CustomerRegulations]
    gw --> Regs
    [trait::CustomerAccounts]
    gw --> Accounts
}
```

Below is an example of a nexus node in the above `domain`s. It centrally orchestrates multiple `domain` sub-nodes during an `OrderRequest` in C++ code. This is much desirable compared to a long chains or pipelines of nodes, each of which would take partial responsibility for completing the request, ultimately making ownership of the response unclear, implicit, and spread out across multiple nodes.

Instead, `ShopRestAPI` takes ownership of the entire flow of the order, delegating the responsibility for the smaller self-contained units of work to other nodes which take no responsibility for completing the overall order response. This looser form of coupling also allows more fine-grained testing, as each testable unit of behaviour is contained within fewer nodes.

```cpp
struct ShopRestAPI
{
    template<class Context>
    struct Node : di::Node
    {
        using Traits = di::Traits<Node
            , trait::ShopRequest
            , trait::DeliveryResponse
        >;

        using OrderRequest = Context::Root::OrderRequestType;
        using RequestId = std::size_t;

        void apply(trait::ShopRequest::order, OrderRequest req)
        {
            // Gateway coordinates the order flow by delegating to the other nodes
            getNode(trait::deliveryRequest).newDelivery(req.id, req.productId, req.quantity);
            getNode(trait::customerRequest).deductFunds(req.id, req.customerId, req.productId, req.quantity);
            newOrderFlows[req.id].req = std::move(req);
        }

        void apply(trait::DeliveryResponse::newDelivery, RequestId reqId, NewDelivery res)
        {
            // Record delivery response
            auto& flow = newOrderFlows[reqId];
            flow.delivery = std::move(res);
            // Respond to client if we received all responses
            if (flow.respond())
                getNode(trait::shopResponse).order(reqId, makeResponse(flow))
        }

        // ... etc

        ShopRespose makeResponse(NewOrderFlow const& flow)
        {
            ShopRespose res;
            if (not *flow.customerValid)
            {
                res.success = false;
                return res;
            }
            // ... build response

            // If we are responding with success, start the order in the backend
            if (res.success)
            {
                getNode(trait::orderRequest).startOrder(
                    flow.req.id,
                    flow.req.customerId,
                    flow.req.productId,
                    flow.req.quantity);
            }
            return res;
        }

        struct NewOrderFlow
        {
            OrderRequest req;
            std::optional<bool> customerValid;
            std::optional<NewDelivery> delivery;

            bool respond() const
            {
                return customerValidId and delivery;
            }
        };

        std::map<RequestId, NewOrderResponses> newOrderFlows;
    };
}
```
