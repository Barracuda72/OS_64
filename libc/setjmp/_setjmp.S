/*
 * _setjmp.c
 *
 * Часть стандарта C2011
 *
 * Функция сохраняет контекст для дальнейшего использования процедурой longjmp
 *
 */

/* 
 * int _setjmp(jmp_buf env)
 */
.globl _setjmp
_setjmp:
  // Согласно x86-64 ABI, сохраняем регистры %rbp, %rbx, %rsp и %r12 - %r15
  mov %r12,   (%rdi)
  mov %r13,  8(%rdi)
  mov %r14, 16(%rdi)
  mov %r15, 24(%rdi)
  mov %rbp, 32(%rdi)
  mov %rbx, 40(%rdi)
  mov %rsp, 48(%rdi)
  mov (%rsp), %rax
  mov %rax, 56(%rdi)
  xor %rax, %rax
  ret
