#include "llvm/IR/Constant.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace llvm::PatternMatch;

namespace {

class LocalConstPropPass : public PassInfoMixin<LocalConstPropPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool changed = false;

    for (auto &BB : F) {
      for (auto I = BB.begin(), E = BB.end(); I != E;) {
        Instruction *Inst = &*I++;

        // example: fold binary ops with two constant integer operands
        if (auto *Bin = dyn_cast<BinaryOperator>(Inst)) {
          if (auto *C0 = dyn_cast<ConstantInt>(Bin->getOperand(0)))
            if (auto *C1 = dyn_cast<ConstantInt>(Bin->getOperand(1))) {
              Constant *Folded = ConstantExpr::get(Bin->getOpcode(), C0, C1);
              Bin->replaceAllUsesWith(Folded);
              Bin->eraseFromParent();
              changed = true;
            }
        }
        /* Extend: ICmp, Select, Phi, ... */
      }
    }
    return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};

} // anonymous namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo

llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "local-constprop",

          LLVM_VERSION_STRING, [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "local-constprop") {
                    FPM.addPass(LocalConstPropPass());
                    return true;
                  }
                  return false;
                });
          }};
}
