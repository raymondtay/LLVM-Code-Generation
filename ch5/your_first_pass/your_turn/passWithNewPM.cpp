#include "llvm/IR/Function.h"
#include "llvm/Support/Debug.h" // For errs().

#include "passWithNewPM.h"

using namespace llvm;

extern bool solutionConstantPropagation(llvm::Function &);

PreservedAnalyses
YourTurnConstantPropagationNewPass::run(Function &F,
                                        FunctionAnalysisManager &FM) {
  errs() << "Solution New PM on " << F.getName() << "\n";

  bool MadeChanges = solutionConstantPropagation(F);
  if (!MadeChanges)
    return PreservedAnalyses::all();

  // Even if we made changes, we did not touched the CFG.
  // So everything on that is still current.
  PreservedAnalyses PA;
  PA.preserveSet<CFGAnalyses>();
  return PA;
}
