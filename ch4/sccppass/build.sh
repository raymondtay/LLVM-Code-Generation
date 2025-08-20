#!/bin/sh

cmake -B build -DLLVM_DIR=$(llvm-config --cmakedir) .
cmake --build build
