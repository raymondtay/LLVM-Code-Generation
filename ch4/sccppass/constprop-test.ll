; RUN: opt -load-pass-plugin %shlib -passes=local-constprop -S %s -o - | FileCheck %s
;
; When the pass runs we expect “2 + 3” and “%a * 4” to be folded away.

define i32 @test(i32 %x) {
entry:
  %a = add i32 2, 3          ; 2 + 3  => 5
  %b = mul i32 %a, 4         ; 5 * 4  => 20
  %c = add i32 %b, %x
  ret i32 %c
}

; CHECK-LABEL: @test(
; CHECK-NOT:   add i32 2, 3
; CHECK-NOT:   mul i32 5, 4
; CHECK:       %c = add i32 20, %x
; CHECK:       ret i32 %c


