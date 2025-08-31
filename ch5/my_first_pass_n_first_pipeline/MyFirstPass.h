#pragma once
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"

using namespace llvm;

struct HelloFunctionPass : PassInfoMixin<HelloFunctionPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
};

// The analysis result type
struct FnStats {
  unsigned NumBlocks = 0;
  unsigned NumInsts = 0;
};


struct FnStatsAnalysis : AnalysisInfoMixin<FnStatsAnalysis> {
  using Result = FnStats;
  Result run(Function &F, FunctionAnalysisManager &);
  static AnalysisKey Key;
};

struct CountBBsAnalysis : PassInfoMixin<CountBBsAnalysis> {
  struct Result { unsigned NumBBs; };
  static AnalysisKey Key;
  Result run(Function &F, FunctionAnalysisManager &);
};


struct PrintFnStatsPass : PassInfoMixin<PrintFnStatsPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

// Optional: if you also have a module pass in MyFirstPass.cpp
struct CountGlobalsModulePass : PassInfoMixin<CountGlobalsModulePass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &);
};

// Custom "print IR" function pass
struct DumpFunctionPass : PassInfoMixin<DumpFunctionPass> { PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
};

