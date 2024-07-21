/*
 * Простейший интерфейс для spinlock'ов
 * spinlock_init
 * spinlock_acquire
 * spinlock_release
 */

.text
.code64

.globl spinlock_init
// void spinlock_init(void *sp);
  spinlock_init:
  movb $1, (%rdi)
  ret

.globl spinlock_acquire
// void spinlock_acquire(void *sp);
spinlock_acquire:
  push %ax
lock:
  xor %al, %al
  xchg %al, (%rdi)
  and %al, %al
  jz lock
  pop %ax
  ret

.globl spinlock_release
// void spinlock_release(void *sp);
spinlock_release:
  movb $1, (%rdi)
  ret

