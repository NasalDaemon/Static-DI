#!/usr/bin/python3

import jinja2

from lark import Lark, Tree, Token
# from lark.load_grammar import load_grammar
from lark.visitors import Visitor_Recursive
from lark.reconstruct import Reconstructor

from functools import cached_property

import os
import os.path
import sys

dirPath = os.path.dirname(os.path.realpath(__file__))
inputFile, outputFile = sys.argv[1:]

assert inputFile.endswith('.dig'), inputFile

if inputFile.endswith('.ixx.dig'):
    isModule = True
    grammarFile = f'{dirPath}/dig_module.lark'
elif inputFile.endswith('.hxx.dig'):
    isModule = False
    grammarFile = f'{dirPath}/dig_header.lark'
else:
    assert False, f"{inputFile} has incorrect extension: must end with .ixx.dig or .hxx.dig"

digParser = Lark.open(grammarFile, maybe_placeholders=False, parser='lalr', cache=True)

# Workaround until this PR is merged: https://github.com/lark-parser/lark/pull/1506
# if not hasattr(digParser, "grammar"):
#     with open(grammarFile, 'r') as file:
#         digParser.grammar, used_files = load_grammar(file.read(), file.name, digParser.options.import_paths, digParser.options.keep_all_tokens)

reconstuctor = Reconstructor(digParser)

with open(inputFile, 'r') as file:
    parsed = digParser.parse(file.read())

def imported(larkRule: str):
    return f'dig__{larkRule}'

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
    def __init__(self, name: str, impl: str):
        self.name: str = name
        if name == '..':
            self.context = "Context"
        else:
            assert name[0].islower(), name
            self.context: str = f'{name[0].upper()}{name[1:]}_'
        self.impl: str = impl
        self.repeaters: list[Repeater] = []
        self.connections: list[Connection] = []

    def addConnection(self, connection: Connection):
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

class Cluster:
    def __init__(self, name: str):
        self.name = name
        self.templates: list[tuple[CppType, str]] = []
        self.parentNode = Node("..", f"{self.name}<Context>")
        self.userNodes: list[Node] = []
        self.repeaters: list[Repeater] = []
        self.nodes: list[Node | Repeater] = []
        self.aliases: list[tuple[str, str]] = []

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
                raise SyntaxError(f'Unknown cluster template: {c.data}')

    def walk(self, children):
        aliases: dict[str, str] = {}
        nodes: dict[str, Node] = {}
        nodes[".."] = self.parentNode
        leftTrait: str
        rightTrait: str
        for child in children:
            if child.data == imported('node'):
                name = child.children[0].value
                assert name not in nodes, (name, nodes)
                impl = reconstuctor.reconstruct(child.children[1])
                nodes[name] = Node(name, impl)
            elif child.data == imported('connection_block'):
                for child in child.children:
                    if child.data == imported('connection_context'):
                        for child in child.children:
                            alias, traitType = child.children
                            typeString = reconstuctor.reconstruct(traitType)
                            if alias in aliases:
                                assert aliases.get(alias) == typeString, f"Alias '{alias}' changed from {aliases.get(alias)} to {typeString}"
                            else:
                                aliases[alias] = typeString
                    elif child.data == imported('connection_trait'):
                        leftTrait = reconstuctor.reconstruct(child.children[0])

                        if len(child.children) == 1:
                            rightTrait = leftTrait
                        else:
                            rightTrait = reconstuctor.reconstruct(child.children[1])
                    elif child.data == imported('connection'):
                        for i in range(0, len(child.children) - 1, 2):
                            lnames, arrow, rnames = (child.children[i], child.children[i+1], child.children[i+2])
                            lnodes = [nodes[name.value] for name in lnames.children]
                            rnodes = [nodes[name.value] for name in rnames.children]

                            lrnodes = ((lnode, rnode) for rnode in rnodes for lnode in lnodes)
                            if arrow.data == imported('left_arrow'):
                                for lnode, rnode in lrnodes:
                                    rnode.addConnection(Connection(context=lnode.context, trait=leftTrait))
                            elif arrow.data == imported('right_arrow'):
                                for lnode, rnode in lrnodes:
                                    lnode.addConnection(Connection(context=rnode.context, trait=rightTrait))
                            elif arrow.data == imported('bi_arrow'):
                                for lnode, rnode in lrnodes:
                                    rnode.addConnection(Connection(context=lnode.context, trait=leftTrait))
                                    lnode.addConnection(Connection(context=rnode.context, trait=rightTrait))
                            elif arrow.data == imported('left_arrow_from'):
                                fromTrait = reconstuctor.reconstruct(arrow.children[0].children[0])
                                toTrait = leftTrait
                                for lnode, rnode in lrnodes:
                                    rnode.addConnection(Connection(context=lnode.context, trait=fromTrait, toTrait=toTrait))
                            elif arrow.data == imported('right_arrow_from'):
                                fromTrait = reconstuctor.reconstruct(arrow.children[0].children[0])
                                toTrait = rightTrait
                                for lnode, rnode in lrnodes:
                                    lnode.addConnection(Connection(context=rnode.context, trait=fromTrait, toTrait=toTrait))
                            elif arrow.data == imported('left_arrow_to'):
                                fromTrait = leftTrait
                                toTrait = reconstuctor.reconstruct(arrow.children[0].children[0])
                                for lnode, rnode in lrnodes:
                                    rnode.addConnection(Connection(context=lnode.context, trait=fromTrait, toTrait=toTrait))
                            elif arrow.data == imported('right_arrow_to'):
                                fromTrait = rightTrait
                                toTrait = reconstuctor.reconstruct(arrow.children[0].children[0])
                                for lnode, rnode in lrnodes:
                                    lnode.addConnection(Connection(context=rnode.context, trait=fromTrait, toTrait=toTrait))
                            elif arrow.data == imported('left_arrow_both'):
                                toTrait = reconstuctor.reconstruct(arrow.children[0].children[0])
                                fromTrait = reconstuctor.reconstruct(arrow.children[1].children[0])
                                for lnode, rnode in lrnodes:
                                    rnode.addConnection(Connection(context=lnode.context, trait=fromTrait, toTrait=toTrait))
                            elif arrow.data == imported('right_arrow_both'):
                                fromTrait = reconstuctor.reconstruct(arrow.children[0].children[0])
                                toTrait = reconstuctor.reconstruct(arrow.children[1].children[0])
                                for lnode, rnode in lrnodes:
                                    lnode.addConnection(Connection(context=rnode.context, trait=fromTrait, toTrait=toTrait))
                            else:
                                raise SyntaxError(f'Unknown arrow: {arrow.data}')
                    else:
                        raise SyntaxError(f'Unknown connection section: {child.data}')
            else:
                raise SyntaxError(f'Unknown cluster section: {child.data}')

        self.userNodes = [node for node in nodes.values() if node.name != '..']
        self.aliases = sorted(aliases.items())
        self.parentNode.connections.sort(key = lambda v : v.trait)
        self.repeaters.extend(self.parentNode.repeaters)
        for node in self.userNodes:
            node.connections.sort(key = lambda v : v.trait)
            self.repeaters.extend(node.repeaters)
        self.nodes.extend(self.userNodes)
        self.nodes.extend(self.repeaters)

class Method:
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
                raise SyntaxError(f'Unknown method template: {c.data}')

    @cached_property
    def isTemplate(self):
        if self.templates or next((True for t, n in self.params if t.isAuto), False):
            return True
        return False

    def dependsOnTypes(self, typesName: str | None):
        if not typesName:
            return False
        if typesName in self.returnType.str:
            return True
        if next((t for t, n in self.templates if typesName in t.str), None):
            return True
        if next((t for t, n in self.params if typesName in t.str), None):
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
            elif c.data == imported('cpp_func_params'):
                for c in c.children:
                    type = CppType.fromTree(c.children[0])
                    name = c.children[1].value
                    self.params.append((type, name))
            elif c.data == imported('method_qualifier'):
                self.isConst = True
            else:
                raise SyntaxError(f'Unknown method entity: {c.data}')

class Trait:
    def __init__(self, name: str):
        self.name: str = name[0].upper() + name[1:]
        self.variable: str = name[0].lower() + name[1:]
        self.typesName: str | None = None
        self.rootName: str | None = None
        self.methods: list[Method] = []
        self.methodNames: list[str] = []
        self.requires: list[str] = []
        self.hasConstRequires = False
        self.hasMutableRequires = False

    def walk(self, children):
        for c in children:
            if c.data == imported('trait_annotations'):
                for ann in c.children:
                    if ann.children[0].value == "Types":
                        self.typesName = ann.children[-1].value
                    elif ann.children[0].value == "Root":
                        self.rootName =  ann.children[-1].value
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
                raise SyntaxError(f'Unknown trait entity: {c.data}')
        self.methods.sort(key = lambda v : (v.name, v.params))
        self.methodNames = sorted(set(method.name for method in self.methods))
        self.hasConstRequires = next((method.isConst for method in self.methods if method.isConst and not method.isTemplate), False)
        self.hasMutableRequires = next((not method.isConst for method in self.methods if not method.isConst and not method.isTemplate), False)

class Namespace:
    def __init__(self, name: str):
        self.name: str = name
        self.traitNames: set[str] = set()
        self.traits: list[Trait] = []
        self.traitAliases: list[list[str]] = []
        self.clusterNames: set[str] = set()
        self.clusters: list[Cluster] = []

    def walk(self, children):
        for c in children:
            if c.data == imported('cluster'):
                hasTemplate = not isinstance(c.children[0], Token)
                nameIndex = 1 if hasTemplate else 0
                name = c.children[nameIndex].value
                assert name not in self.clusterNames, (name, self.clusterNames)
                self.clusterNames.add(name)
                cluster = Cluster(name)
                if hasTemplate:
                    cluster.addTemplate(c.children[0])
                cluster.walk(c.children[(nameIndex+1):])
                self.clusters.append(cluster)
            elif c.data == imported('trait_def'):
                name = c.children[0].value
                assert name not in self.traitNames, (name, self.traitNames)
                if not name[0].isupper():
                    raise SyntaxError(f'First character of trait name {name} is not Uppercase')
                self.traitNames.add(name)
                trait = Trait(name)
                trait.walk(c.children[1:])
                self.traits.append(trait)
            elif c.data == imported('trait_alias'):
                name = c.children[0].value
                assert name not in self.traitNames, (name, self.traitNames)
                if not name[0].isupper():
                    raise SyntaxError(f'First character of trait name {name} is not Uppercase')
                self.traitAliases.append([t.value for t in c.children])
            else:
                raise SyntaxError(f'Unknown namespace entity: {c.data}')
        self.clusters.sort(key = lambda v : v.name)
        self.traits.sort(key = lambda v : v.name)

class HeaderRepr:
    def __init__(self, parsed):
        self.inputFile = inputFile
        self.includes: list[str] = []
        self.namespaces: list[Namespace] = {}
        self.hasCluster = False
        self.hasTrait = False
        self.walk(parsed)

    def walk(self, parsed):
        includes: set[str] = set()
        namespaces: dict[str, Namespace] = {}
        for t in parsed.children:
            if t.data == 'include':
                includes.add(t.children[0].value)
            elif t.data == 'namespace':
                name = t.children[0].value
                if name not in namespaces:
                    namespaces[name] = Namespace(name)
                namespaces[name].walk(t.children[1:])
            else:
                raise SyntaxError('Unknown token: %s' % t.data)
        self.includes = sorted(includes)
        self.namespaces = sorted(namespaces.values(), key = lambda v : v.name)
        self.hasCluster = next((True for namespace in self.namespaces if len(namespace.clusters) != 0), False)
        self.hasTrait = next((True for namespace in self.namespaces if len(namespace.traits) != 0), False)

class ModuleRepr:
    def __init__(self, parsed):
        self.inputFile = inputFile
        self.includes: list[str] = []
        self.exportModule: str
        self.importModules: list[tuple[str, str]] = []
        self.namespaces: list[Namespace] = {}
        self.hasCluster = False
        self.hasTrait = False
        self.walk(parsed)

    def walk(self, parsed):
        includes: set[str] = set()
        importModules: set[str] = set()
        namespaces: dict[str, Namespace] = {}
        for t in parsed.children:
            if t.data == 'includes':
                for include in t.children:
                    includes.add(include.children[0].value)
            elif t.data == 'export_module':
                self.exportModule = t.children[0].value
            elif t.data == 'import_module':
                importModules.add((t.children[0].value, t.children[1].value))
            elif t.data == 'namespace':
                name = t.children[0].value
                if name not in namespaces:
                    namespaces[name] = Namespace(name)
                namespaces[name].walk(t.children[1:])
            else:
                raise SyntaxError('Unknown token: %s' % t.data)
        self.includes = sorted(includes)
        self.importModules = sorted(importModules)
        self.namespaces = sorted(namespaces.values(), key = lambda v : v.name)
        self.hasCluster = next((True for namespace in self.namespaces if len(namespace.clusters) != 0), False)
        self.hasTrait = next((True for namespace in self.namespaces if len(namespace.traits) != 0), False)

templateLoader = jinja2.FileSystemLoader(searchpath=dirPath)
templateEnv = jinja2.Environment(loader=templateLoader, trim_blocks=True, lstrip_blocks=True)
if isModule:
    template = templateEnv.get_template("template.ixx.jinja")
    outputText = template.render(repr=ModuleRepr(parsed), export="export ")
else:
    template = templateEnv.get_template("template.hxx.jinja")
    outputText = template.render(repr=HeaderRepr(parsed), export="")

with open(outputFile, 'a+') as file:
    file.seek(0)
    currentText = file.read()
    if currentText != outputText:
        print(f"DIG {"created" if currentText == "" else "changed"}: {outputFile}")
        file.truncate(0)
        file.write(outputText)
