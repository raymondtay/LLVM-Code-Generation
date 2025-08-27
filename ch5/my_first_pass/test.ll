; ModuleID = 'test'
source_filename = "test.c"

define i32 @foo(i32 %x) {
entry:
  %add = add i32 %x, 42
  ret i32 %add
}

define void @bar() {
entry:
  ret void
}


