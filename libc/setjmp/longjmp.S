/*
 * longjmp.c
 *
 * Часть стандарта C2011
 *
 * Функция восстанавливает контекст, сохраненный процедурой setjmp
 *
 */

/*
 * _Noreturn void longjmp(jmp_buf env, int val)
 */
.globl longjmp
longjmp:
  mov   (%rdi), %r12
  mov  8(%rdi), %r13
  mov 16(%rdi), %r14
  mov 24(%rdi), %r15
  mov 32(%rdi), %rbp
  mov 40(%rdi), %rbx
  mov 48(%rdi), %rsp
  mov 56(%rdi), %rax
  mov %rax, (%rsp)
  cmp $0, %rsi
  jne longjmp1
  mov $1, %rsi
longjmp1:
  mov %rsi, %rax
  ret
