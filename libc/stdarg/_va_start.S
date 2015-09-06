/*
 * _va_start.c
 *
 * Часть стандарта C2011
 *
 * Функция инициализирует список аргументов переменной длинны
 *
 */

/* 
 * void _va_start(va_list *ap, void *p, int size)
 */
.globl _va_start
_va_start:
  mov %rbp, %rax
  mov $120, %rcx // Не уходим сильно в стек, у функций редко бывает много
_va_start1:     // параметров
  sub $4, %rax
  cmp %rsi, (%rax)
  je _va_start_found // Нашли аргумент
  loop _va_start1

  xor %rax, %rax // Не нашли ничего
  jmp _va_start_ret

_va_start_found:
  // sub %rdx, %rax
  add $0x20, %rax
  
_va_start_ret:
  mov %rax, (%rdi)
  ret
