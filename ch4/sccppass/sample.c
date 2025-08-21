/* sample.c
 *
 * Compile to IR with:
 *   clang -O0 -S -emit-llvm sample.c -o sample.ll
 */

#include <stdint.h>

/* 1. Straight-line algebraic folds */
int test(int x) {
  int a = 2 + 3; /* 5 */
  int b = a * 4; /* 20 */
  return b + x;  /* constant-fold should leave only 20 + x */
}

/* 2. Boolean logic – should fold to a simple return 1 */
int always_true(void) {
  int flag = (10 > 3) && (7 != 0);
  return flag;
}

/* 3. Constant-condition branch – SCCP / your extended pass
 *    should delete the dead ‘else’ block. */
int select_const(void) {
  if (0)       /* provably false */
    return 42; /* dead */
  else
    return 13; /* live code */
}

/* 4. Folding through a simple loop induction variable.
 *    Your basic local pass won't catch this; SCCP will. */
int sum_first_n(void) {
  const int N = 4; /* constant */
  int total = 0;
  for (int i = 0; i < N; ++i)
    total += i; /* should become total = 0+1+2+3 = 6 */
  return total;
}
