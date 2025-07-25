#!/usr/bin/python3

import argparse
import jinja2

from lark import Lark, Tree, Token, UnexpectedInput
# from lark.load_grammar import load_grammar
from lark.visitors import Visitor_Recursive
from lark.reconstruct import Reconstructor

from functools import cached_property

from pathlib import Path

dirPath = Path(__file__).resolve().parent

argParser = argparse.ArgumentParser(
                    prog='Static-DI generator',
                    description='Generates cpp source files from Static-DI DSL called dig')

argParser.add_argument('-i', '--input')
argParser.add_argument('-o', '--output')
argParser.add_argument('-m', '--module', action='store_true')

args = argParser.parse_args()

inputFile: str = args.input
inputPath = Path(inputFile).resolve()
outputFile: str = args.output
isModule: bool = args.module
isEmbedded: bool = inputPath.suffixes[-1] != '.dig'

grammarFile = dirPath.joinpath(dirPath, 'dig_module.lark' if isModule else 'dig_header.lark')
digParser = Lark.open(grammarFile, maybe_placeholders=False, parser='lalr', cache=True)

# Workaround until this PR is merged: https://github.com/lark-parser/lark/pull/1506
# if not hasattr(digParser, "grammar"):
#     with open(grammarFile, 'r') as file:
#         digParser.grammar, used_files = load_grammar(file.read(), file.name, digParser.options.import_paths, digParser.options.keep_all_tokens)

reconstuctor = Reconstructor(digParser)

with open(inputFile, 'r') as file:
    text = file.read()
    if isEmbedded:
        sections = []
        begin = 'di-embed-begin'
        end = 'di-embed-end'
        while True:
            beginPos = text.find(begin)
            if beginPos == -1:
                assert sections, f"'{begin}' not found in {inputPath}"
                break
            beginPos += len(begin)
            endPos = text.find(end, beginPos)
            assert endPos != -1, f"matching '{end}' not found in {inputPath}"

            sections.append(text[beginPos:endPos])
            text = text[endPos + len(end):]
        text = "\n".join(sections)

    def onParseError(e: UnexpectedInput) -> bool:
        print(f"{inputPath}:{e.line}:{e.column} parse error:\n{e.get_context(text)}")
        return False

    parsed = digParser.parse(text, on_error=onParseError)


def imported(larkRule: str):
    return f'dig__{larkRule}'


def getPos(t: Tree | Token | None) -> str:
    if t is None:
        return str(inputPath)
    if isinstance(t, Token):
        return f"{inputPath}:{t.line}:{t.column}"
    if isinstance(t, Tree):
        return getPos(t.children[0])
    else:
        raise TypeError(f"Expected Tree or Token, got {type(t)}")


class AddColonToRequiresStatements(Visitor_Recursive):
    def dig__cpp_requires_statement(self, tree):
        if not isinstance(tree.children[-1], Token) or tree.children[-1].type != imported('SEMICOLON'):
            tree.children.append(Token(imported('SEMICOLON'), ';'))

addColonToRequiresStatements = AddColonToRequiresStatements()


class CppType:
    def __init__(self, string: str, *, isAuto: bool = False, tree: Tree | None = None):
        if string.endswith("..."):
            self.str = string.removesuffix("...")
            self.pack = "..."
        else:
            self.str = string
            self.pack = ""
        self._isAuto = isAuto
        self.tree = tree

    @classmethod
    def fromTree(cls, tree):
        str = reconstuctor.reconstruct(tree)
        return cls(str, tree = tree)

    @cached_property
    def isAuto(self):
        if self.tree is None:
            return self._isAuto
        return bool(next(self.tree.scan_values(lambda t : t.value == "auto"), None))

    def __repr__(self):
        return self.str

    def __lt__(self, other):
        return (self.str, self.pack) < (other.str, other.pack)


class Repeater:
    def __init__(self, name: str, trait: str, id: int):
        if name == "..":
            name = "parent"
        assert name[0].islower(), name
        self.name = f'_{name}Repeater{id}'
        self.isParent = False
        self.isNexus = False
        self.isUnary = True
        self.hasState = False
        self.trait = trait
        self.connections: list['Connection'] = []
        self.context = f'{name[0].upper()}{name[1:]}Repeater{id}_'

    @property
    def impl(self):
        return f'di::Repeater<{self.trait}, {len(self.connections)}>'

    def addConnection(self, connection: 'Connection'):
        assert connection.trait == self.trait, (connection.trait, self.trait)
        connection.trait = f"di::RepeaterTrait<{len(self.connections)}>"
        self.connections.append(connection)


class Connection:
    def __init__(self, context, trait, toTrait: str | None = None, toRepeater: Repeater | None = None):
        self.context: str = context
        self.trait: str = trait
        self.toTrait: str = toTrait or trait
        self.toRepeater = toRepeater

    def copy(self):
        return Connection(self.context, self.trait, self.toTrait, self.toRepeater)

    def isRenaming(self):
        return self.trait != self.toTrait


class Node:
    def __init__(self, name: str, tree: Tree | None, impl: str, cluster: 'Cluster | Domain', isFirst: bool):
        self.repeaters: list[Repeater] = []
        self.connections: list[Connection] = []
        self.clients: list[tuple[Node, str]] = []
        self.name: str = name
        self.impl: str = impl
        self.cluster = cluster
        self.isNexus: bool = isFirst and isinstance(cluster, Domain)
        if name == '..':
            self.isParent = True
            self.context = "Context"
        else:
            self.isParent = False
            self.isUnary = name.upper() != name
            self.hasState = name[0].isupper()
            self.context: str = name + '_'
            if self.isNexus and not self.isUnary:
                raise SyntaxError(f"{getPos(tree)} Nexus node '{name}' in '{cluster.fullName}' must be a unary node")

    def addConnection(self, pos, isOverride: bool, toNode: 'Node', trait: str, *, toTrait: str | None = None):
        assert self.cluster == toNode.cluster
        if error := self.cluster.getConnectionError(self, toNode, isOverride):
            raise SyntaxError(f"{pos} Cannot connect '{self.name}' to '{toNode.name}' in {self.cluster.clusterClass} '{self.cluster.fullName}': {error}")

        toNode.addClient(self, trait)
        connection = Connection(toNode.context, trait=trait, toTrait=toTrait)
        if existing := next((c for c in self.connections if c.trait == connection.trait), None):
            if existing.toRepeater is None:
                repeater = Repeater(self.name, connection.trait, len(self.repeaters))
                repeater.addConnection(existing.copy())
                repeater.addConnection(connection)
                existing.context = repeater.context
                existing.toTrait = existing.trait
                existing.toRepeater = repeater
                self.repeaters.append(repeater)
            else:
                existing.toRepeater.addConnection(connection)
        else:
            self.connections.append(connection)

    def addClient(self, client, trait):
        self.clients.append((client, trait))


class Cluster:
    def __init__(self, name: str, namespace: "Namespace"):
        self.name = name
        self.namespace = namespace
        self.templates: list[tuple[CppType, str]] = []
        self.contextName: str = "Context"
        self.rootName: str | None = None
        self.infoName: str | None = None
        self.parentNode = Node("..", None, self.name, cluster=self, isFirst=False)
        self.userNodes: list[Node] = []
        self.repeaters: list[Repeater] = []
        self.nodes: list[Node | Repeater] = []
        self.aliases: list[tuple[str, str]] = []
        self.dependencies: list[str] = []

    @property
    def clusterType(self) -> str:
        return "di::Cluster"

    @property
    def clusterClass(self) -> str:
        return "cluster"

    @property
    def fullName(self) -> str:
        if self.namespace.name:
            return f"{self.namespace.name}::{self.name}"
        else:
            return self.name

    def predicates(self, node) -> list[str]:
        return []

    def predicatesStr(self, node) -> str:
        if preds := self.predicates(node):
            return ", " + ", ".join(preds)
        else:
            return ""

    def nodeName(self):
        if self.templates:
            return "Node"
        else:
            return f'{self.name}_'

    def addTemplate(self, token):
        for c in token.children:
            if c.data == imported('tparam_type'):
                self.templates.append((CppType(c.children[0].value.replace("typename", "class")), c.children[1].value))
            elif c.data == imported('tparam_non_type'):
                type = CppType.fromTree(c.children[0])
                self.templates.append((type, c.children[1].value))
            else:
                raise SyntaxError(f'{getPos(c)} Unknown cluster template: {c.data}')

    def getConnectionError(self, lnode: Node, rnode: Node, isOverride: bool) -> None:
        return None

    def arrowSign(self, arrow: Tree) -> str:
        return next((c.value for c in arrow.children if isinstance(c, Token)))

    def validateArrow(self, arrow: Tree):
        sign = self.arrowSign(arrow)
        chevrons = max(sign.count('<'), sign.count('>'))
        if chevrons > 1:
            raise SyntaxError(f"{getPos(arrow)} Only one chevron is allowed per arrow in clusters.")
        return False

    def walk(self, children):
        aliases: dict[str, str] = {}
        nodes: dict[str, Node] = {}
        nodes[".."] = self.parentNode
        leftTrait: str
        rightTrait: str
        for child in children:
            if child.data == imported('cluster_annotations'):
                for ann in child.children:
                    if ann.children[-1].value == "Context":
                        self.contextName = ann.children[0].value
                    elif ann.children[-1].value == "Root":
                        self.rootName = ann.children[0].value
                    elif ann.children[-1].value == "Info":
                        self.infoName = ann.children[0].value
                    else:
                        raise SyntaxError(f"{getPos(ann)} Unknown cluster annotation: {ann.children[0].value}")
            elif child.data == imported('node'):
                name = child.children[0].value
                if name in nodes:
                    raise SyntaxError(f"{getPos(child)} Node '{name}' already defined in cluster '{self.fullName}'")
                impl = reconstuctor.reconstruct(child.children[1])
                if len(child.children) > 2:
                    for wrapper in child.children[2:]:
                        cls = wrapper.children[0].value
                        if len(wrapper.children) > 1:
                            args = [impl]
                            args.extend(reconstuctor.reconstruct(arg) for arg in wrapper.children[1].children[1:-1:2])
                            impl = f"{cls}<{", ".join(args)}>"
                        else:
                            impl = f"{cls}<{impl}>"
                isFirst = len(nodes) == 1
                nodes[name] = Node(name, child, impl, cluster=self, isFirst=isFirst)
            elif child.data == imported('connection_block'):
                for child in child.children:
                    if child.data == imported('connection_context'):
                        for child in child.children:
                            alias, traitType = child.children
                            typeString = reconstuctor.reconstruct(traitType)
                            if alias in aliases:
                                if aliases.get(alias) != typeString:
                                    raise SyntaxError(f"{getPos(alias)} Alias '{alias}' changed from {aliases.get(alias)} to {typeString}")
                            else:
                                aliases[alias] = typeString
                    elif child.data == imported('connection_trait'):
                        leftTrait = reconstuctor.reconstruct(child.children[0])

                        if len(child.children) == 1:
                            rightTrait = leftTrait
                        else:
                            rightTrait = reconstuctor.reconstruct(child.children[-1])
                    elif child.data == imported('connection'):
                        for i in range(0, len(child.children) - 1, 2):
                            lnames, arrow, rnames = (child.children[i], child.children[i+1], child.children[i+2])
                            isOverride = self.validateArrow(arrow)
                            pos = getPos(arrow)
                            lnodes = [nodes[name.value] for name in lnames.children]
                            rnodes = [nodes[name.value] for name in rnames.children]

                            lrnodes = ((lnode, rnode) for rnode in rnodes for lnode in lnodes)
                            if arrow.data == imported('left_arrow'):
                                for lnode, rnode in lrnodes:
                                    rnode.addConnection(pos, isOverride, lnode, leftTrait)
                            elif arrow.data == imported('right_arrow'):
                                for lnode, rnode in lrnodes:
                                    lnode.addConnection(pos, isOverride, rnode, rightTrait)
                            elif arrow.data == imported('bi_arrow'):
                                for lnode, rnode in lrnodes:
                                    rnode.addConnection(pos, isOverride, lnode, leftTrait)
                                    lnode.addConnection(pos, isOverride, rnode, rightTrait)
                            elif arrow.data == imported('left_arrow_from'):
                                fromTrait = reconstuctor.reconstruct(arrow.children[-1].children[0])
                                toTrait = leftTrait
                                for lnode, rnode in lrnodes:
                                    rnode.addConnection(pos, isOverride, lnode, fromTrait, toTrait=toTrait)
                            elif arrow.data == imported('right_arrow_from'):
                                fromTrait = reconstuctor.reconstruct(arrow.children[0].children[0])
                                toTrait = rightTrait
                                for lnode, rnode in lrnodes:
                                    lnode.addConnection(pos, isOverride, rnode, fromTrait, toTrait=toTrait)
                            elif arrow.data == imported('left_arrow_to'):
                                fromTrait = leftTrait
                                toTrait = reconstuctor.reconstruct(arrow.children[0].children[0])
                                for lnode, rnode in lrnodes:
                                    rnode.addConnection(pos, isOverride, lnode, fromTrait, toTrait=toTrait)
                            elif arrow.data == imported('right_arrow_to'):
                                fromTrait = rightTrait
                                toTrait = reconstuctor.reconstruct(arrow.children[-1].children[0])
                                for lnode, rnode in lrnodes:
                                    lnode.addConnection(pos, isOverride, rnode, fromTrait, toTrait=toTrait)
                            elif arrow.data == imported('left_arrow_both'):
                                toTrait = reconstuctor.reconstruct(arrow.children[0].children[0])
                                fromTrait = reconstuctor.reconstruct(arrow.children[-1].children[0])
                                for lnode, rnode in lrnodes:
                                    rnode.addConnection(pos, isOverride, lnode, fromTrait, toTrait=toTrait)
                            elif arrow.data == imported('right_arrow_both'):
                                fromTrait = reconstuctor.reconstruct(arrow.children[0].children[0])
                                toTrait = reconstuctor.reconstruct(arrow.children[-1].children[0])
                                for lnode, rnode in lrnodes:
                                    lnode.addConnection(pos, isOverride, rnode, fromTrait, toTrait=toTrait)
                            else:
                                raise SyntaxError(f'{pos} Unknown arrow: {arrow.data}')
                    else:
                        raise SyntaxError(f'{getPos(child)} Unknown connection section: {child.data}')
            else:
                raise SyntaxError(f'{getPos(child)} Unknown cluster section: {child.data}')

        self.userNodes = [node for node in nodes.values() if node.name != '..']
        self.aliases = sorted(aliases.items())
        self.parentNode.connections.sort(key=lambda v : v.trait)
        self.repeaters.extend(self.parentNode.repeaters)
        for node in self.userNodes:
            node.connections.sort(key=lambda v : v.trait)
            self.repeaters.extend(node.repeaters)
        self.nodes.extend(self.userNodes)
        self.repeaters.sort(key=lambda r : r.name)
        self.nodes.extend(self.repeaters)
        self.dependencies = [aliases.get(trait, trait) for _, trait in self.parentNode.clients]
        self.dependencies.sort()

    def finalize(self):
        self.dependencies = [f"{dep}*" for dep in self.dependencies]


class Domain(Cluster):
    def __init__(self, name: str, namespace: "Namespace"):
        super().__init__(name, namespace)
        self.extraChevronsSeen = 0
        self.overridesAllowed = 0
        self.overridesSeen = 0
        self.minExtraChevrons = 2
        self.overridesPerExtraChevron = 2

    @property
    def clusterType(self) -> str:
        return "di::Domain<di::DomainParams{.MaxDepth=3}>"

    @property
    def clusterClass(self) -> str:
        return "domain"

    def predicates(self, node: Node | Repeater) -> list[str]:
        preds = ["di::pred::HasDepends"]
        if not node.isUnary:
            preds.append("di::pred::NonUnary")
            return preds
        preds.append("di::pred::Unary")
        if not node.hasState:
            preds.append("di::pred::Stateless")
        return preds

    def validateArrow(self, arrow: Tree) -> bool:
        sign = self.arrowSign(arrow)
        lExtra = sign.count('<') - 1
        rExtra = sign.count('>') - 1
        isOverride = False
        if lExtra > 0:
            isOverride = True
            if self.extraChevronsSeen and lExtra != self.extraChevronsSeen:
                raise SyntaxError(f"{getPos(arrow)} Inconsistent number of extra chevrons in '{self.fullName}'")
            self.extraChevronsSeen = lExtra
        if rExtra > 0:
            isOverride = True
            if self.extraChevronsSeen and rExtra != self.extraChevronsSeen:
                raise SyntaxError(f"{getPos(arrow)} Inconsistent number of extra chevrons in '{self.fullName}'")
            self.extraChevronsSeen = rExtra

        if self.extraChevronsSeen:
            if self.extraChevronsSeen < self.minExtraChevrons:
                raise SyntaxError(f"{getPos(arrow)} Overrides must have at least {1+self.minExtraChevrons} chevrons")
            if sign.count('-') - 1 < self.extraChevronsSeen:
                raise SyntaxError(f"{getPos(arrow)} Overrides must have at least as many dashes ('-') as chevrons ('<' or '>')")
            self.overridesAllowed = self.overridesPerExtraChevron * self.extraChevronsSeen

        return isOverride

    def getConnectionError(self, fromNode: Node, toNode: Node, isOverride: bool) -> str | None:
        # nexus can connect to anything in any direction
        if fromNode.isNexus or toNode.isNexus:
            if isOverride:
                return "No override is needed for this connection, but an override was specified"
            return None

        # non-nexus nodes may not connect to parent in any direction
        if fromNode.isParent or toNode.isParent:
            return "Only nexus node may be connected to parent in domains"

        # unary peers may not connect to anything
        if fromNode.isUnary or toNode.isUnary:
            if isOverride and fromNode.isUnary and toNode.isUnary:
                if self.overridesSeen < self.overridesAllowed:
                    self.overridesSeen += 1
                    return None
                else:
                    return (f"Too many unary-to-unary connections in '{self.name}' ({self.overridesAllowed} allowed) - "
                            "use more chevrons to allow more overrides, or remove some connections")
            return ("Unary nodes may only be connected to the domain nexus. "
                "To explicitly allow a unary-to-unary connection you can override with extra chevrons in the arrow ('<' and/or '>')")

        if isOverride:
            return "No override is needed for this connection, but an override was specified"
        return None

    def finalize(self):
        minOverrides = self.minExtraChevrons * self.overridesPerExtraChevron
        if self.overridesAllowed > minOverrides and (self.overridesAllowed - self.overridesSeen) >= self.overridesPerExtraChevron:
            raise SyntaxError(
                f"{inputFile} "
                f"More extra chevrons used in '{self.fullName}' than is needed for the required number of overrides ({self.overridesSeen} required). "
                f"Each extra chevron allows you {self.overridesPerExtraChevron} more overrides in the domain, so only "
                f"{1+max(self.minExtraChevrons, -(self.overridesSeen // -self.overridesPerExtraChevron))} chevrons are needed in total per override.")


class Method:
    __reserved_names__ = ['impl', 'isTrait']

    def __init__(self):
        self.name: str = "<unknown>"
        self.templates: list[tuple[CppType, str]] = []
        self.returnType: CppType = CppType("decltype(auto)", isAuto=True)
        self.params: list[tuple[CppType, str]] = []
        self.isConst: bool = False

    def addTemplate(self, children):
        for c in children:
            if c.data == imported('tparam_type'):
                self.templates.append((CppType("class"), c.children[0].value))
            elif c.data == imported('tparam_non_type'):
                type = CppType.fromTree(c.children[0])
                self.templates.append((type, c.children[1].value))
            else:
                raise SyntaxError(f'{getPos(c)} Unknown method template: {c.data}')

    @cached_property
    def isTemplate(self):
        if self.templates or next((True for t, n in self.params if t.isAuto), False):
            return True
        return False

    @cached_property
    def isUnconstrainedReturn(self):
        return self.returnType.str == "decltype(auto)" or self.returnType.str == "void" or self.returnType.str.replace("&", "") == "auto"

    @cached_property
    def isAutoReturn(self):
        return self.returnType.isAuto and self.returnType.str != "decltype(auto)"

    def walk(self, children):
        for c in children:
            if c.data == imported('template_params'):
                self.addTemplate(c.children)
            elif c.data == imported('cpp_type'):
                self.returnType = CppType.fromTree(c)
            elif c.data == imported('method_name'):
                self.name = c.children[0].value
                if self.name in self.__reserved_names__:
                    raise SyntaxError(f"{getPos(c)} '{self.name}' cannot be the name of a method, it is reserved for di::TraitView")
            elif c.data == imported('cpp_func_params'):
                for c in c.children:
                    type = CppType.fromTree(c.children[0])
                    name = c.children[1].value
                    self.params.append((type, name))
            elif c.data == imported('method_qualifier'):
                self.isConst = True
            else:
                raise SyntaxError(f'{getPos(c)} Unknown method entity: {c.data}')


class Trait:
    def __init__(self, name: str):
        self.name: str = name[0].upper() + name[1:]
        self.variable: str = name[0].lower() + name[1:]
        self.typesName: str | None = None
        self.rootName: str | None = None
        self.infoName: str | None = None
        self.implName: str = "Impl"
        self.implNamed = False
        self.methods: list[Method] = []
        self.methodNames: list[str] = []
        self.requires: list[str] = []
        self.hasConstRequires = False
        self.hasMutableRequires = False

    def walk(self, children):
        for c in children:
            if c.data == imported('trait_annotations'):
                for ann in c.children:
                    if ann.children[-1].value == "Types":
                        self.typesName = ann.children[0].value
                    elif ann.children[-1].value == "Root":
                        self.rootName =  ann.children[0].value
                    elif ann.children[-1].value == "Info":
                        self.infoName =  ann.children[0].value
                    elif ann.children[-1].value == "Impl":
                        self.implName =  ann.children[0].value
                        self.implNamed = True
                    else:
                        raise SyntaxError(f"{getPos(ann)} Unknown trait annotation: {ann.children[0].value}")
            elif c.data == imported('trait_body'):
                c = c.children[0]
                if c.data == imported('trait_type'):
                    if self.typesName is None:
                        self.typesName = "Types_T_" # use ugly name if not specified to avoid shadowing
                    self.requires.append(f"typename {self.typesName}::{c.children[0].value};")
                elif c.data == imported('trait_root'):
                    if self.rootName is None:
                        self.rootName = "Root_T_" # use ugly name if not specified to avoid shadowing
                    self.requires.append(f"typename {self.rootName}::{c.children[0].value};")
                elif c.data == imported('trait_info'):
                    if self.infoName is None:
                        self.infoName = "Info_T_" # use ugly name if not specified to avoid shadowing
                    self.requires.append(f"typename {self.infoName}::{c.children[0].value};")
                elif c.data == imported('trait_method_signature'):
                    method = Method()
                    method.walk(c.children)
                    self.methods.append(method)
                elif c.data == imported('trait_method_elipsis'):
                    method = Method()
                    method.name = c.children[0].value
                    method.params.append((CppType("auto&&...", isAuto=True), "args"))
                    self.methods.append(method)
                elif c.data == imported('trait_requires'):
                    if c.children[0].data == imported('trait_requires_block'):
                        addColonToRequiresStatements.visit(c.children[0])
                        requires = "requires " + reconstuctor.reconstruct(c.children[0])
                    else:
                        requires = reconstuctor.reconstruct(c.children[0].children[0])
                    if not requires.endswith(';'):
                        requires += ';'
                    self.requires.append(requires)
            else:
                raise SyntaxError(f'{getPos(c)} Unknown trait entity: {c.data}')
        self.methods.sort(key = lambda v : (v.name, v.params))
        self.methodNames = sorted(set(method.name for method in self.methods))
        self.hasConstRequires = next((method.isConst for method in self.methods if method.isConst and not method.isTemplate), False)
        self.hasMutableRequires = next((not method.isConst for method in self.methods if not method.isConst and not method.isTemplate), False)


class Namespace:
    def __init__(self, name: str, repr: 'Repr'):
        self.name: str = name
        self.repr: 'Repr' = repr
        self.traitNames: set[str] = set()
        self.traits: list[Trait] = []
        self.traitAliases: list[list[str]] = []
        self.clusterNames: set[str] = set()
        self.clusters: list[Cluster] = []

    def walk(self, children):
        for c in children:
            if c.data == 'cluster':
                self.repr.visitCluster(self.name, c)
            elif c.data == imported('trait_def'):
                self.repr.visitTraitDef(self.name, c)
            elif c.data == imported('trait_alias'):
                self.repr.visitTraitAlias(self.name, c)
            else:
                raise SyntaxError(f'{getPos(c)} Unknown namespace entity: {c.data}')

    def addCluster(self, name: str, tree: Tree, isDomain: bool) -> Cluster | Domain:
        if name in self.clusterNames:
            raise SyntaxError(f"{getPos(tree)} cluster by name '{name}' already defined in namespace '{self.name}'")
        self.clusterNames.add(name)
        cluster = Domain(name, self) if isDomain else Cluster(name, self)
        self.clusters.append(cluster)
        return cluster

    def addTrait(self, name: str, tree: Tree) -> Trait:
        if name in self.traitNames:
            raise SyntaxError(f"{getPos(tree)} trait by name '{name}' already defined in namespace '{self.name}'")
        if not name[0].isupper():
            raise SyntaxError(f'{getPos(tree)} First character of trait name {name} is not Uppercase')
        self.traitNames.add(name)
        trait = Trait(name)
        self.traits.append(trait)
        return trait

    def addTraitAlias(self, names: list[str], tree: Tree):
        name = names[0]
        if name in self.traitNames:
            raise SyntaxError(f"{getPos(tree)} trait by name '{name}' already defined in namespace '{self.name}'")
        self.traitNames.add(name)
        if not name[0].isupper():
            raise SyntaxError(f'{getPos(tree)} First character of trait name {name} is not Uppercase')
        self.traitAliases.append(names)

    def finalize(self):
        self.clusters.sort(key = lambda v : v.name)
        for c in self.clusters:
            c.finalize()
        self.traits.sort(key = lambda v : v.name)


class Repr:
    def __init__(self, parsed):
        self.inputFile = inputFile
        self.includes: list[str] = []
        self.exportModule: str
        self.importModules: list[str] = []
        self.namespaces: list[Namespace] = []
        self.namespacesDict: dict[str, Namespace] = {}
        self.hasCluster = False
        self.hasTrait = False
        self.walk(parsed)
        self.finalize()

    def walk(self, parsed):
        includes: set[str] = set()
        importModules: set[tuple[str, str]] = set()
        for t in parsed.children:
            if t.data == 'include':
                includes.add(t.children[0].value)
            elif t.data == 'includes':
                for include in t.children:
                    includes.add(include.children[0].value)
            elif t.data == 'export_module':
                self.exportModule = t.children[0].value
            elif t.data == 'import_module':
                importModules.add((t.children[0].value, t.children[1].value))
            elif t.data == 'namespace':
                name = t.children[0].value
                self.getNamespace(name).walk(t.children[1:])
            elif t.data == 'cluster':
                self.visitCluster("", t)
            elif t.data == imported('trait_def'):
                self.visitTraitDef("", t)
            elif t.data == imported('trait_alias'):
                self.visitTraitAlias("", t)
            else:
                raise SyntaxError(f'{getPos(t)} Unknown token: {t.data}')
        self.includes = sorted(includes)
        self.importModules = [f"{impt} {name}" for impt, name in sorted(importModules, key=lambda v: v[1])]

    def visitCluster(self, sourceNs: str, tree: Tree):
        hasTemplate = not isinstance(tree.children[0], Token)
        typeIndex = 1 if hasTemplate else 0
        nameIndex = typeIndex + 1
        name, namespace = self.splitNamespace(tree, sourceNs, tree.children[nameIndex].value)
        cluster = namespace.addCluster(name, tree.children[nameIndex], isDomain=tree.children[typeIndex].value == "domain")
        if hasTemplate:
            cluster.addTemplate(tree.children[0])
        cluster.walk(tree.children[(nameIndex+1):])

    def visitTraitDef(self, sourceNs: str, tree: Tree):
        name, namespace = self.splitNamespace(tree, sourceNs, tree.children[0].value)
        trait = namespace.addTrait(name, tree)
        trait.walk(tree.children[1:])

    def visitTraitAlias(self, sourceNs: str, tree: Tree):
        name, namespace = self.splitNamespace(tree, sourceNs, tree.children[0].value)
        namespace.addTraitAlias([c.value for c in tree.children], tree)

    def splitNamespace(self, tree: Tree, sourceNs: str, fqName: str) -> tuple[str, Namespace]:
        pos = fqName.rfind("::")
        if pos == -1:
            if not sourceNs:
                raise SyntaxError(
                    f"{getPos(tree)} trait/alias may not be defined in root namespace, please namespace-qualify {fqName} "
                    f"as your::ns::{fqName} or wrap {fqName} in namespace your::ns {'{ ... }'}")
            return (fqName, self.getNamespace(sourceNs))
        else:
            namespace = fqName[0:pos]
            if namespace.startswith("::"):
                raise SyntaxError(f"{getPos(tree)} trait/alias namespace-qualifier '{namespace}' may not reference the root namespace")
            if sourceNs:
                namespace = sourceNs + "::" + namespace
            name = fqName[pos+2:]
            return (name, self.getNamespace(namespace))

    def getNamespace(self, name: str) -> Namespace:
        if name not in self.namespacesDict:
            self.namespacesDict[name] = Namespace(name, self)
        return self.namespacesDict[name]

    def finalize(self):
        self.namespaces = sorted(self.namespacesDict.values(), key = lambda v : v.name)
        for n in self.namespaces:
            n.finalize()
        self.hasCluster = next((True for namespace in self.namespaces if len(namespace.clusters) != 0), False)
        self.hasTrait = next((True for namespace in self.namespaces if len(namespace.traits) != 0), False)


templateLoader = jinja2.FileSystemLoader(searchpath=dirPath)
templateEnv = jinja2.Environment(loader=templateLoader, trim_blocks=True, lstrip_blocks=True)
template = templateEnv.get_template(f"template.{'ixx' if isModule else 'hxx'}.jinja")
outputText = template.render(repr=Repr(parsed), export="export " if isModule else "DI_MODULE_EXPORT\n")

with open(outputFile, 'a+') as file:
    file.seek(0)
    currentText = file.read()
    if currentText != outputText:
        print(f"DIG {"created" if currentText == "" else "changed"}: {outputFile}")
        file.truncate(0)
        file.write(outputText)
