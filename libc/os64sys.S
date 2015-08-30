#include <syscall_nr.h>

.globl write
write:
  mov $_N_write, %rax
  int $0x80
  ret

