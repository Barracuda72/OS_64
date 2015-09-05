#include <syscall_nr.h>

.globl write
write:
  mov $_N_write, %rax
  int $0x80
  ret

.globl getpid
getpid:
  mov $_N_getpid, %rax
  int $0x80
  ret

// Заглушка
.globl mremap
mremap:
  ret
