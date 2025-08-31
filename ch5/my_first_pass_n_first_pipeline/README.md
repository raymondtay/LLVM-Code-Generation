1. What is a “Pass”?
===
	• A pass in LLVM is a transformation or analysis over some part of the IR (Intermediate Representation).
	• Examples:
	• Transformations: Constant Propagation, Dead Code Elimination, Inlining.
	• Analyses: Dominator Tree Construction, Alias Analysis.

1.1 Passes run over one of:
====
	• Module (whole program / translation unit),
	• Function (single function),
	• Loop (loop structures),
	• CGSCC (Call Graph SCC).

2.0 What is a pass manager?
===

A pass manager is a driver for the set of passes that you want to run.

It fulfills three main functions:

* It provides a structure to run passes in a specific order.
* It makes sure that the dependencies of a pass are properly executed before the pass itself.
* It preserves or invalidates the various analyses based on the passes’ effects.
* When a pass manager invokes a pass, it also provides specific guarantees on the order in which the IR is visited.

These guarantees are as follows:

* For CGSCC-scoped passes, a pass manager invokes the pass on the leaf
  strongly connected component (SCC) regions first then moves up in the call graph.
* For loop-scoped passes, loops are visited following their nested level from innermost to outermost loops.
* For region-scoped passes, the nesting level is also used in a similar
  fashion: from the innermost region to the outermost regions.
* For other scopes, the order is not specified.


2.1 Why do we need a Pass Manager?
====

LLVM programs don’t run just a single pass.
Instead, you want a pipeline of optimizations, like:
```
Inlining → Constant Propagation → DCE → Loop Unrolling → ...
```

The Pass Manager:
	• Orchestrates execution of passes.
	• Manages dependencies between passes (e.g., DCE needs Dominator Tree).
	• Ensures correctness (e.g., invalidating analyses when IR changes).
	• Provides a way to plug in your own passes.

3. Legacy vs New Pass Manager
===

LLVM historically had a Legacy Pass Manager, but since LLVM 10+, the New Pass Manager (NPM) is standard.

Legacy PM
	• llvm::Pass, llvm::ModulePass, llvm::FunctionPass.
	• Registered via initializeMyPass(PassRegistry&).

New PM (recommended)
	• Cleaner, faster, explicit.
	• Uses templates and concepts instead of deep inheritance.
	• Passes look like simple structs with run() methods.

