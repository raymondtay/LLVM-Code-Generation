; ModuleID = 'test'
source_filename = "test.c"

@G1 = global i32 0
@G2 = constant i8 42

define i32 @foo(i32 %x) {
entry:
  %add = add i32 %x, 42
  ret i32 %add
}

define void @bar() {
entry:
  %a = alloca i64
  store i64 7, i64* %a
  ret void
}

