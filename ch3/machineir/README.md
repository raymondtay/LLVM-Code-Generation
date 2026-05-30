# Building Machine IR #

In this exercise, you will build your first function using the Machine intermediate representation (IR).

Open `your_turn/populate_function.cpp` and implement the `populateMachineIR` function according to the comment
in that file.

We recommend to start by running the program once to see the output of the solution.

This way you will get an idea of which opcodes you should use for your machine instructions.

At the end, your produced IR should match what is printed between the `# Machine code for function solution_foo` and `# End machine code for function solution_foo.` lines.

Your function is printed between the `# Machine code for function foo` and `# End machine code for function foo.` lines.

## Configuring your environment ##


```bash
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=<path/to/llvm/install>/lib/cmake/llvm -Bbuild .
ninja -Cbuild
```

## Running the example ##

```bash
./build/build_mir
```

## Solution ##

If your output doesn't match the solution, look at `solution/populate_function.cpp` to see how to implement the desired IR.

## Understanding SESE Constraint

The SESE constraint can be violated within the terminator's region to save some memory space, 
as seen in the implementation in ch3/machineir/solution/populate_function.cpp. This is a design
chioce in the Machine IR, where the block with two terminators violates the SESE constraint.

### Verfiying SESE Constraint

It is possible to generate a version of the Machine IR that does not violate this constraint by
creating a fall-through block with just one unconditional branch in it. However, this means
allocating one more MachineBasicBlock object for just one instruction. The trade-off at violating
the SESE constraint is show below.

| BB1 violates SESE | More code is required to respect SESE |
| ----------------- | ------------------------------------- | 
| BB1:                      | BB1:                          |
|  G_BRCOND predicate, BB2  |  G_BRCOND predicate, BB2      |
|  G_BR BB3                 | BBTmp:                        |
|                           |  G_BR BB3                     |

### Using the Verifier

The verified can be used to check if the IR passes without issue, and it's recommended to try to have
your IR pass the verifier without issue. By using the verifier, one can ensure that the SESE constraint
is not violated in the LLVM code.

