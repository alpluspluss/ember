## Introduction

Sprout IR is an intermediate representation in static single assignment form (SSA) 
for optimizing compilers. It is compact and hierarchical so data dependency tracking and other forms
of static analysis such as alias analysis, partial redundancy elimination, etc becomes natural
to perform; hence it is named "sprout" as it organically grows from the flat graph representation.

## Motivation

Sprout IR was primarily designed to reduce memory inefficiencies as traditional SSA-based IRs 
have significant metadata overhead, particularly for versioning and phi nodes at merge points. 
As compact graph-based representations, it allows for sophisticated analyses compared to pure tree or
pure graphs that either makes optimizations difficult or hard to navigate; all this while maintaining
locality. Naturally by being hierarchical, the IRs are capable of being processed in parallel passes
and directly encodes dependencies in the structure; this reduces a lot of additional analyses.

## Structure

Sprout IR is a directed graph where nodes represent computations, values, and operations. Edges
represent data flow dependencies. Each node has direct links to both its inputs and users. Unlike
traditional SSA, Sprout IR doet **not** introduce variable versioning for each assignment;
it directly represents values and their relationships instead.

Nodes are organized into hierarchical regions. Each region contains a set of nodes and regions 
can contain nested child regions. They naturally represent scopes, basic blocks, functions, and modules;
and provides efficient traversal and context for analysis.

Control flow is encoded in two complementary ways, either through region nesting implicitly or
control dependency edges explicitly.

Formally, Sprout IR is your average directed graph, $G = (V, E)$, where $V$ is the set of vertices 
representing computational values and, $E \subseteq V \times V$, is the set of directed edges 
representing data dependencies. Each node $v \in V$ has three properties:

```tex
type(v) ∈ {Constant, Parameter, Operation, Control, ...}
inputs(v) = {u ∈ V | (u, v) ∈ E}
users(v) = {w ∈ V | (v, w) ∈ E}
```

The hierarchy is defined as a tree $T = (R, C)$ where $R$ is the set of regions and 
$C \subseteq R \times R$ is the set of containment relationships. Each region $r \in R$ has the 
following properties:

```tex
nodes(r) ⊆ V
children(r) = {s ∈ R | (r, s) ∈ C}
parent(r) = {p ∈ R | (p, r) ∈ C} ∪ {undefined}
```

Control flow is encoded in the structure through a combination of region containments which are regions
$r_2$ contained in region $r_1$ implies control flow from $r_1$ to $r_2$, control dependencies 
which are nodes or region may be explicitly marked as control-dependent on a control node.

