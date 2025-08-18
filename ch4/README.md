A `phi` instruction copies the input value from the related control flow edge
to the definition of the phi. For instance, in ![SSA form of a program](./images/program_SSA_form.png), when the
control flow goes from `BB1` to `BB3` (that is, the left edge – the first value of the phi),
`var1` is copied into `var3`. When the control flow goes from `BB2` to `BB3`,
the second value, `var2`, is copied into `var3`.

To put it differently, `phi` instructions act as if a copy were inserted
on the related incoming edge.

In the LLVM IR and Machine IR levels, `phi` instructions are grouped together
at the beginning of _basic blocks_. It is invalid to insert non-`phi`
instructions before `phi` instructions.

`Phi` instructions are mapped to the `PHINode` class, which is a subclass of
the `Instruction` class, in the LLVM IR and to the `MachineInstr` class
with `getOpcode() == PHI` in the Machine IR.

The optimization in the LLVM IR responsible for promoting memory locations to
SSA values is `mem2reg`.

# Def-use and use-def chains

`Def-use` and `use-def` chains are fundamental concepts in data flow analysis,
enabling the efficient tracking of value definitions and uses in programming.

`Def-use` chains refer to the relationship between a definition of a value and
its subsequent uses, allowing for efficient tracking of value usage.
This concept is essential in data flow analysis, as it enables the
identification of value sources and sinks.

`Use-def` chains, on the other hand, represent the relationship between a value's
use and its definition, enabling the identification of value sources.
By analyzing these chains, developers can optimize program performance, reduce errors,
and enhance code maintainability.

The following table gives an example of code where the code snippet on the right 
would jump from the scope of bar to the scope of foo, simply by following the 
`use-def` chain starting at `bar_res`:

| Input IR | A program using LLVM APIs |
| ---------| --------------------------| 
| extern char *global;         | Value *Global = BarRes.getOperand(0); | 
|char **other_global = &global;| for (User *UserOfGlobal : Global->users()) {| 
|char foo() {                  |   auto *UserInstr = dyn_cast<Instruction>(UserOfGlobal);| 
|  char foo_res = global[0];   |   if (!UserInstr) {| 
|  return foo_res;             |     errs() << "Found a non-instruction use of global: " << *UserOfGlobal << '\n';| 
|}                             |     continue;| 
|char bar() {                  |   }                                                              | 
|  char bar_res = global[0];   |   Function *UserFunc = UserInstr->getParent()->getParent();      | 
|  return bar_res;             |   if (UserFunc != BarFunc) {                                     |
|}                             |     errs() << "Went from bar to " << UserFunc->getName() << '\n';|
|                              |   }                                                              |
|                              | }
