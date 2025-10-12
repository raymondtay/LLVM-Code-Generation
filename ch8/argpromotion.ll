; Argument Promotion: This pass tries to replace pointer arguments with regular values.
; This replacement is only possible if the function is internal to the module, meaning 
; that it is okay to change its ABI because te optimization sees all the uses of the function.
;
; This transformation exposes opportunities to get rid of stack allocation
; and memory accesses.

define i64 @foo() {
  %local = alloca i64
  store i64 2, ptr %local
  %res = call i64 @bar(ptr %local)
  ret i64 %res
}

define internal i64 @bar(ptr %local) {
  %val = load i64, ptr %local
  %res = add i64 %val, 2
  ret i64 %res
}

