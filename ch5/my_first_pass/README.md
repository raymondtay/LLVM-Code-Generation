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

2. Why do we need a Pass Manager?
===

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

