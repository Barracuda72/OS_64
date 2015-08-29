#include <syscall_nr.h>

.globl write
write:
  mov $_N_write, %rax
  xchg %bx, %bx
  int $0x80
  ret

