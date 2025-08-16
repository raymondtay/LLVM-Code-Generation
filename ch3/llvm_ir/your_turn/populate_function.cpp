#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"    // For ConstantInt.
#include "llvm/IR/DerivedTypes.h" // For PointerType, FunctionType.
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h" // For errs().

#include <memory> // For unique_ptr

using namespace llvm;

// The goal of this function is to build a Module that
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
// The IR for this snippet (at O0) is:
// define void @foo(i32 %arg, i32 %arg1) {
// bb:
//   %i = alloca i32
//   %i2 = alloca i32
//   %i3 = alloca i32
//   store i32 %arg, ptr %i
//   store i32 %arg1, ptr %i2
//   %i4 = load i32, ptr %i
//   %i5 = load i32, ptr %i2
//   %i6 = add i32 %i4, %i5
//   store i32 %i6, ptr %i3
//   %i7 = load i32, ptr %i3
//   %i8 = icmp eq i32 %i7, 255
//   br i1 %i8, label %bb9, label %bb12
//
// bb9:
//   %i10 = load i32, ptr %i3
//   call void @bar(i32 %i10)
//   %i11 = call i32 @baz()
//   store i32 %i11, ptr %i3
//   br label %bb12
//
// bb12:
//   %i13 = load i32, ptr %i3
//   call void @bar(i32 %i13)
//   ret void
// }
//
// declare void @bar(i32)
// declare i32 @baz(...)
std::unique_ptr<Module> myBuildModule(LLVMContext &Ctx) {
 
  // 1. Create the types needed
  Type* VoidTy = Type::getVoidTy(Ctx);
  Type* Int32Ty = Type::getInt32Ty(Ctx);
  Type* PtrTy = PointerType::get(Ctx, /* AddressSpace */ 0);


  // 2. Create the high-level module
  std::unique_ptr<Module> mod =
    std::make_unique<Module>("Solution", Ctx);


  // 3.1 Create the function 'baz'
  FunctionType* bazTy =
    FunctionType::get(/*Return Type*/ Int32Ty, /*isVarArg=*/ false);
  Function* bazF =
    cast<Function>(mod->getOrInsertFunction("baz", bazTy).getCallee());

  // 3.2 Create the function 'bar'
  FunctionType* barTy =
    FunctionType::get(VoidTy, ArrayRef(Int32Ty), false);
  Function* barF =
    cast<Function>(mod->getOrInsertFunction("bar", barTy).getCallee());

  // 3.3 Create the function 'foo'
  FunctionType* fooTy =
    FunctionType::get(VoidTy, ArrayRef({Int32Ty, Int32Ty}), false);
  Function* fooF =
    cast<Function>(mod->getOrInsertFunction("foo", fooTy).getCallee());

  // 4. Once the functions are created, then we create the basic blocks
  //   since it starts at Foo, remember the single entry single exit analysis
  //   then its easy to see that we need 3 basicblocks.
  BasicBlock* BB = BasicBlock::Create(Ctx, "bb", fooF);
  BasicBlock* BB2 = BasicBlock::Create(Ctx, "bb2", fooF); // THEN branch "bar(var); var = baz()"
  BasicBlock* BB3 = BasicBlock::Create(Ctx, "bb3", fooF); // ELSE branch "bar(var)"

  // 4.1 Populate bb
  IRBuilder Builder(BB);
  Value* I = Builder.CreateAlloca(Int32Ty); // create space for local variable
  Value* I2 = Builder.CreateAlloca(Int32Ty); // create space for local variable
  Value* I3 = Builder.CreateAlloca(Int32Ty); // create space for local variable

  // Get the function foo's passed-in arguments
  Value* firstArg = fooF->getArg(0); // get 1st argument
  Value* secondArg = fooF->getArg(1); // get 2nd argument
 
  // Store and associate the values with the correct types.
  Builder.CreateStore(firstArg, I);
  Builder.CreateStore(secondArg, I2);

  // Reload from the local variables (this means its in the CPU registers)
  Value* I4 = Builder.CreateLoad(Int32Ty, I);
  Value* I5 = Builder.CreateLoad(Int32Ty, I2);

  Value* I6 = Builder.CreateAdd(I4, I5); // Perform the addition op.
  Builder.CreateStore(I6, I3); // Store the outcome to I3

  // Create a temporary value that houses the outcome of the comparison op.
  Value* I7 = Builder.CreateLoad(Int32Ty, I3);
  // Create the value that represents 0xFF, perform the comp-op and store into I8
  Value* Constant255 = ConstantInt::get(Int32Ty, 255);
  Value* I8 = Builder.CreateICmpEQ(I7, Constant255);

  // If I8 == true, go BB2 else BB3
  Builder.CreateCondBr(I8, BB2, BB3);

  // ------------ Populate the BB of the THEN branch ------------------
  // Reset the builder on the next basic block.
  Builder.SetInsertPoint(BB2);
  // Reload the local variable i3.
  Value *I10 = Builder.CreateLoad(Int32Ty, I3);
  // Call bar with i10.
  Builder.CreateCall(barF->getFunctionType(), barF, ArrayRef(I10));
  // Call baz.
  Value *I11 = Builder.CreateCall(bazF->getFunctionType(), bazF);
  // Store the result in the local variable i3.
  Builder.CreateStore(I11, I3);
  // Jump to the next block.
  Builder.CreateBr(BB3);


  // ------------ Populate the BB of the ELSE branch ------------------
  // Reset the builder on the next basic block.
  Builder.SetInsertPoint(BB3);
  // Reload the local variable I3.
  Value *I13 = Builder.CreateLoad(Int32Ty, I3);
  // Call bar on i13.
  Builder.CreateCall(barF->getFunctionType(), barF, ArrayRef(I13));
  // Create the final return.
  // Remember all basic block must end with a terminator.
  Builder.CreateRetVoid();

  return mod;
}
