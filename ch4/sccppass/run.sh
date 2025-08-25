#!/bin/sh

# LOCAL_SRC - Indicate which sample files to test
# DEST_SRC - Indicate which file to write the output to
# LIB - Indicate the library of the plugin.
LOCAL_SRC=./tests/Inputs/sample.c
DEST_SRC=./tests/Inputs/sample.ll
LIB=./build/LocalConstProp.dylib

clang -O0 -S -emit-llvm $LOCAL_SRC -o $DEST_SRC
opt -load-pass-plugin $LIB -passes=local-constprop -S $DEST_SRC -o -
