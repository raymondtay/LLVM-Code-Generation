; Dead Argument Elimination (DAE) is similar to Argument Promotion pass
; in that it can only happen if the optimization sees all the call sites
; of the related function.
; 
; In this example, we see that `ptr %local2` is never used as its never invoked
; at their call sites (at the moment, its just 1 call-site); hence it can 
; be optimized away

define i64 @foo() {
  %local = alloca i64
  %local2 = alloca i64
  store i64 2, ptr %local
  store i64 2, ptr %local2
  %res = call i64 @bar(ptr %local, ptr %local2)
  ret i64 %res
}

define internal i64 @bar(ptr %local, ptr %local2) {
  %val = load i64, ptr %local
  %res = add i64 %val, 2
  ret i64 %res
}

