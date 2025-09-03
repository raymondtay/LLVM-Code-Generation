#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h" // For the new PassManager.
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Transforms/IPO/AlwaysInliner.h"       // For inliner pass.
#include "llvm/Transforms/InstCombine/InstCombine.h" // For instcombine pass.
#include "llvm/Transforms/Utils/Mem2Reg.h"           // For mem2reg pass.

using namespace llvm;

void runYourTurnPassPipelineForNewPM(Module &MyModule) {

  LLVMContext &context = MyModule.getContext();

  // CAREFUL : the order of the manager is important here since the
  // destructor needs to be called in the right order otherwise
  // it will crash
  FunctionAnalysisManager FAM;
  ModuleAnalysisManager MAM;

  PassInstrumentationCallbacks PIC;
  PrintPassOptions PrintPassOpts;
  PrintPassOpts.Verbose = true;
  PrintPassOpts.SkipAnalyses = true;
  PrintPassOpts.Indent = true;

  StandardInstrumentations SI(context,
                              /*DebugLogging=*/true,
                              /*VerifyEach=*/false, PrintPassOpts);

  SI.registerCallbacks(PIC, &MAM);

  // Register the passes used implicitly at the start of the pipeline
  // and enable logging.
  MAM.registerPass([&] { return PassInstrumentationAnalysis(&PIC); });
  FAM.registerPass([&] { return PassInstrumentationAnalysis(&PIC); });

  MAM.registerPass([&] { return FunctionAnalysisManagerModuleProxy(FAM); });
  FAM.registerPass([&] { return ModuleAnalysisManagerFunctionProxy(MAM); });

  ModulePassManager newPM;
  FunctionPassManager fpMgr;

  // Populate the XXXAnalysisManager with the IDs for all the passes.

  PassBuilder PB;
  PB.registerFunctionAnalyses(FAM);
  PB.registerModuleAnalyses(MAM);

#ifndef USE_MODULE_MGR
  fpMgr.addPass(PromotePass());
  fpMgr.addPass(InstCombinePass());
#else
  newPM.addPass(createModuleToFunctionPassAdaptor(PromotePass()));
  newPM.addPass(createModuleToFunctionPassAdaptor(InstCombinePass()));
#endif

  newPM.addPass(createModuleToFunctionPassAdaptor(std::move(fpMgr)));
  newPM.addPass(AlwaysInlinerPass());
  newPM.run(MyModule, MAM);
}
