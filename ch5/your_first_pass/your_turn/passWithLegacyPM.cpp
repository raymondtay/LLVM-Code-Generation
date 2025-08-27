#include "llvm/IR/Function.h"
#include "llvm/Pass.h"          // For FunctionPass & INITIALIZE_PASS.
#include "llvm/Support/Debug.h" // For errs().

using namespace llvm;

extern bool solutionConstantPropagation(llvm::Function &);

// The implementation of this function is generated at the end of this file. See
// INITIALIZE_PASS.
namespace llvm {
void initializeYourTurnConstantPropagationPass(PassRegistry &);
};

namespace {
class YourTurnConstantPropagation : public FunctionPass {
public:
  static char ID;
  YourTurnConstantPropagation() : FunctionPass(ID) {
    // Technically speaking, we do not need to put this in the constructor.
    // usually this call lives in the uber InitializeAllXXX.
    initializeYourTurnConstantPropagationPass(*PassRegistry::getPassRegistry());
  }

  // Main function of a FunctionPass.llvm
  bool runOnFunction(Function &F) override {
    errs() << "Solution Legacy called on " << F.getName() << '\n';
    return solutionConstantPropagation(F);
  }
};
} // End anonymous namespace.

char YourTurnConstantPropagation::ID = 0;

INITIALIZE_PASS(YourTurnConstantPropagation, "legacy-solution",
                "Legacy Solution", false, false);

Pass *createYourTurnPassForLegacyPM() {
  return new YourTurnConstantPropagation();
}
