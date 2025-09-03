// RunPipeline.cpp
#include "MyFirstPass.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

// Using the New PassManager
// https://llvm.org/docs/NewPassManager.html
//
static cl::opt<std::string>
    InputFilename(cl::Positional, cl::desc("<input IR file>"), cl::Required);

int main(int argc, char **argv) {
  InitLLVM X(argc, argv);
  cl::ParseCommandLineOptions(argc, argv, "Mini pipeline runner\n");

  LLVMContext Context;

  // --- Load module ---
  SMDiagnostic Err;
  std::unique_ptr<Module> M = parseIRFile(InputFilename, Err, Context);
  if (!M) {
    Err.print(argv[0], errs());
    return 1;
  }

  // --- Managers ---
  PassBuilder PB;
  LoopAnalysisManager LAM;
  FunctionAnalysisManager FAM;
  CGSCCAnalysisManager CGAM;
  ModuleAnalysisManager MAM;

  // Register the AA pipeline (good practice)
  FAM.registerPass([&] { return PB.buildDefaultAAPipeline(); });

  // Register the Analysis object to the Analysis Manager
  FAM.registerPass([] { return FnStatsAnalysis(); });

  // --- Instrumentation you requested ---
  PassInstrumentationCallbacks PIC;
  PrintPassOptions PrintPassOpts;
  PrintPassOpts.Verbose = true;
  PrintPassOpts.SkipAnalyses = false;
  PrintPassOpts.Indent = true;

  // `true` enables debug logging to stderr for each pass
  StandardInstrumentations SI(Context,
                              /*DebugLogging=*/true,
                              /*VerifyEachPass=*/false, PrintPassOpts);
  SI.registerCallbacks(PIC, &MAM);

  // Make instrumentation analysis available at the module layer.
  MAM.registerPass([&] { return PassInstrumentationAnalysis(&PIC); });

  // Wire managers into PassBuilder (also registers cross-manager proxies)
  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  // If your passes are in a plugin, you can still *use* them via pipeline names
  // once the plugin is loaded by opt. For this standalone runner, we’ll just
  // build a pipeline that references your compiled passes if they’re in-tree;
  // otherwise, replace with built-ins for quick testing.

  // --- Build the pipeline you used in opt ---
  // Top-level is a Module pipeline; run module pass then per-function passes.
  ModulePassManager MPM;

  // (A) If you compiled your custom passes into this executable:
  extern ModulePassManager buildCustomModulePM();     // (optional)
  extern FunctionPassManager buildCustomFunctionPM(); // (optional)
  // Otherwise, inline minimal examples here:

  // Example: your module pass
  // extern PassPluginLibraryInfo llvmGetPassPluginInfo(); // if you kept plugin
  // style If you directly have a C++ class CountGlobalsModulePass, you can do:
  //   MPM.addPass(CountGlobalsModulePass());
  MPM.addPass(CountGlobalsModulePass());

  // For demonstration, we’ll use the function adaptor and print IR:
  FunctionPassManager FPM;
  // FPM.addPass(HelloFunctionPass());
  // FPM.addPass(PrintFnStatsPass());
  FPM.addPass(HelloFunctionPass());
  FPM.addPass(PrintFnStatsPass());

  // Dump function IR after each function pass (optional, shows that PIC works)
  FPM.addPass(DumpFunctionPass());

  MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));

  // --- Run it ---
  MPM.run(*M, MAM);

  // If you want to write the (possibly transformed) module to stdout:
  // M->print(outs(), nullptr);

  return 0;
}
