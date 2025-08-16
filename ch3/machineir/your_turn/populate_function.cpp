#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineFrameInfo.h" // For CreateStackObject.
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineMemOperand.h" // For MachinePointerInfo.
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/TargetOpcodes.h"     // For INLINEASM.
#include "llvm/CodeGenTypes/LowLevelType.h" // For LLT.
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h" // For ICMP_EQ.

using namespace llvm;

// The goal of this function is to build a MachineFunction that
// represents the lowering of the following foo, a C function:
// extern int baz();
// extern void bar(int);
// void foo(int a, int b) {
//   int var = a + b;
//   if (var == 0xFF) {
//     bar(var);
//     var = baz();
//   }
//   bar(var);
// }
//
// The proposed ABI is:
// - 32-bit arguments are passed through registers: w0, w1
// - 32-bit returned values are passed through registers: w0, w1
// w0 and w1 are given as argument of this Function.
//
// The local variable named var is expected to live on the stack.
MachineFunction *populateMachineIR(MachineModuleInfo &MMI, Function &Foo,
                                   Register W0, Register W1) {
  MachineFunction &MF = MMI.getOrCreateMachineFunction(Foo);

  // The type for bool.
  LLT I1 = LLT::scalar(1);
  // The type of var.
  LLT I32 = LLT::scalar(32);

  // To use to create load and store for var.
  MachinePointerInfo PtrInfo;
  Align VarStackAlign(4);
  // The type for the address of var.
  LLT VarAddrLLT = LLT::pointer(/*AddressSpace=*/0, /*SizeInBits=*/64);

  // The stack slot for var.
  int FrameIndex = MF.getFrameInfo().CreateStackObject(32, VarStackAlign,
                                                       /*IsSpillSlot=*/false);

  // TODO: Populate MF.

  // Create the 3 basic blopcks that compose Foo. In my code its called
  // BB, BB2, BB3.
  MachineBasicBlock *EntryBB = MF.CreateMachineBasicBlock();
  MachineBasicBlock *ThenBB = MF.CreateMachineBasicBlock();
  MachineBasicBlock *ElseBB = MF.CreateMachineBasicBlock();
  MF.push_back(EntryBB);
  MF.push_back(ThenBB);
  MF.push_back(ElseBB);

  // Create the ControlFlowGraph and it should resemble
  // the llvm_ir exercise.
  EntryBB->addSuccessor(EntryBB);
  EntryBB->addSuccessor(ElseBB);
  ThenBB->addSuccessor(ElseBB);

  // -------------------
  //
  MachineIRBuilder miBuilder(*EntryBB, EntryBB->end());
  // Get the input arguments
  Register regA = miBuilder.buildCopy(I32, W0).getReg(0);
  Register regB = miBuilder.buildCopy(I32, W1).getReg(0);
  // Get the stack slot for the variable
  Register varStackAddr =
      miBuilder.buildFrameIndex(VarAddrLLT, FrameIndex).getReg(0);

  // Add.
  Register regAdd = miBuilder.buildAdd(I32, regA, regB).getReg(0);
  // Write result to var's address.
  miBuilder.buildStore(regAdd, varStackAddr, PtrInfo, VarStackAlign);
  // Build the int-comparison
  Register const0xFF = miBuilder.buildConstant(I32, 0xFF).getReg(0);
  Register reloadedVar0 =
      miBuilder.buildLoad(I32, varStackAddr, PtrInfo, VarStackAlign).getReg(0);
  Register cmp =
      miBuilder.buildICmp(CmpInst::ICMP_EQ, I1, reloadedVar0, const0xFF)
          .getReg(0);

  // Conditional branch. If true jump to ThenBB
  miBuilder.buildBrCond(cmp, *ThenBB);
  // else jump to ElseBB
  miBuilder.buildBr(*ElseBB);

  // ThenBB
  // Reset MLBuilder to point at the end of ThenBB
  miBuilder.setInsertPt(*ThenBB, ThenBB->end());
  // put var in w0 for the call to `bar`.
  Register reloadedVar1 =
      miBuilder.buildLoad(I32, varStackAddr, PtrInfo, VarStackAlign).getReg(0);
  miBuilder.buildCopy(W0, reloadedVar1);

  // Fake call to `bar`.
  miBuilder.buildInstr(TargetOpcode::INLINEASM, {}, {})
      .addExternalSymbol("bl @bar")
      .addImm(0)
      .addReg(W0, RegState::Implicit);
  // Fake call to `baz`.
  miBuilder.buildInstr(TargetOpcode::INLINEASM, {}, {})
      .addExternalSymbol("bl @baz")
      .addImm(0)
      .addReg(W0, RegState::Implicit | RegState::Define);

  // Copy the result of baz to bar
  Register resultOfBaz = miBuilder.buildCopy(I32, W0).getReg(0);
  miBuilder.buildStore(resultOfBaz, varStackAddr, PtrInfo, VarStackAlign);
  // Fallthrough to ExitB, no need for a terminator.

  // ExitBB
  // Reset MIBuilder to point at the end of ExitBB.
  miBuilder.setInsertPt(*ElseBB, ElseBB->end());
  // Put var in W0 for the call to bar.
  Register reloadedVar2 =
      miBuilder.buildLoad(I32, varStackAddr, PtrInfo, VarStackAlign).getReg(0);
  miBuilder.buildCopy(W0, reloadedVar2);

  // Fake call to bar.
  miBuilder.buildInstr(TargetOpcode::INLINEASM, {}, {})
      .addExternalSymbol("bl @bar")
      .addImm(0)
      .addReg(W0, RegState::Implicit);
  // End of the function, return void;
  miBuilder.buildInstr(TargetOpcode::INLINEASM, {}, {})
      .addExternalSymbol("ret")
      .addImm(0);

  return &MF;
}
