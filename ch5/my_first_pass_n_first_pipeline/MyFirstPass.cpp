#include "MyFirstPass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// A simple function pass
PreservedAnalyses HelloFunctionPass::run(Function &F, FunctionAnalysisManager &) {
  errs() << "Hello from: " << F.getName() << "\n";
  return PreservedAnalyses::all();
}

// Count the BasicBlocks encountered
CountBBsAnalysis::Result CountBBsAnalysis::run(Function &F, FunctionAnalysisManager &) {
  return { static_cast<unsigned>(F.size()) };
}

AnalysisKey CountBBsAnalysis::Key;

PreservedAnalyses CountGlobalsModulePass::run(Module &M, ModuleAnalysisManager &) {
  unsigned globals = 0, constGlobals = 0;
  for (const GlobalVariable &G : M.globals()) {
    ++globals;
    if (G.isConstant())
      ++constGlobals;
  }
  errs() << "[count-globals] total=" << globals << " const=" << constGlobals
         << "\n";
  // We only read the module, so we preserve everything.
  return PreservedAnalyses::all();
}

// Custom "print IR" function pass
PreservedAnalyses DumpFunctionPass::run(Function &F, FunctionAnalysisManager &) {
  outs() << "[IR dump for function: " << F.getName() << "]\n";
  F.print(outs());
  return PreservedAnalyses::all();
}


// ---------------------------
// (C) Function Analysis + consumer
// ---------------------------

// The analysis proper
FnStatsAnalysis::Result FnStatsAnalysis::run(Function &F, FunctionAnalysisManager &) {
  FnStatsAnalysis::Result R{};
  for (auto &BB : F) {
    ++R.NumBlocks;
    R.NumInsts += (unsigned)BB.size();
  }
  return R;
}

AnalysisKey FnStatsAnalysis::Key;

// A function pass that **consumes** the analysis
PreservedAnalyses PrintFnStatsPass::run(Function &F, FunctionAnalysisManager &AM) {
  const auto &R = AM.getResult<FnStatsAnalysis>(F);
  errs() << "[fn-stats] " << F.getName()
         << " blocks=" << R.NumBlocks
         << " insts=" << R.NumInsts << "\n";
  // We don't mutate IR:
  return PreservedAnalyses::all();
}

// Register the pass plugin
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "hello-pass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            // 1) Make our Function analysis available to the PM.
            PB.registerAnalysisRegistrationCallback(
                [](FunctionAnalysisManager &FAM) {
                  FAM.registerPass([] { return FnStatsAnalysis(); });
                });

            // 2) Register pipeline keywords → attach our passes.

            // Function pass: "hello-func"
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "hello-func") {
                    FPM.addPass(HelloFunctionPass());
                    return true;
                  }
                  if (Name == "fn-stats") {
                    FPM.addPass(PrintFnStatsPass());
                    return true;
                  }
                  return false;
                });
            // Module pass: "module-count-globals"
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "module-count-globals") {
                    MPM.addPass(CountGlobalsModulePass());
                    return true;
                  }
                  return false;
                });
          }};
}
