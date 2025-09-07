# Getting started with TableGen

The name `TableGen` stems from its original usage - generating tables.
For instance, TableGen generates the table that represents all the registers of a target.
TableGen outgrew this purpose and is now used to model a wide range of things,
from Clang's command-line options to multi-level intermediate representation (MLIR)
operation's boilerplate C++ code, or used directly within LLVM to generate
the instruction selection tables, and so on.

Fundamentally, TableGen is a DSL to produce records. A record is an entity with a
name and an arbitrary number of fields, where each field has its own type.

How these records are used and what output TableGen generates from them depends on the
specific TableGen backend.

TableGen's strength lies in how you can structure the generation of your records
such that you can factor out the repeated parts of records.

---

This directory contains examples of TableGen inputs.

The goal here is to familiarize yourself with the TableGen syntax and understand how the records are expanded.

## Configuring your build environment

```bash
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=<path/to/llvm/install>/lib/cmake/llvm -Bbuild .
```

This will initialize your build directory in `build` (the `-B` option) with Ninja (`-G` option).

You must have a version of LLVM installed at `<path/to/llvm/install>` for this to succeed.
Either build and install your own llvm (with the `install` target from your LLVM build) or install an [official LLVM release](https://releases.llvm.org/) package.

## Build

```bash
ninja -C build
```

This builds the default target in the build directory.

This should produce in the `build` directory one `.inc` file per `.td` file in the source directory.

Look at the produced `.inc` and the related `.td` and try to see if the expansion matches your understanding after reading Chapter 6.
