/*
 * linsys.S
 *
 * Обертка системных вызовов Linux 
 * для hosted-режима
 *
 */

#include "/usr/include/asm/unistd_64.h"

// ssize_t write(int fd, const void *buf, size_t count)
.globl write
write:
  mov $__NR_write, %rax
  syscall
  ret

// ssize_t read(int fd, void *buf, size_t count)
.globl read
read:
  mov $__NR_read, %rax
  syscall
  ret

// int open(const char *pathname, int flags)
.globl open
open:
  mov $__NR_open, %rax
  syscall
  ret

// int close(int fd)
.globl close
close:
  mov $__NR_close, %rax
  syscall
  ret

// off_t lseek(int fd, off_t offset, int whence)
.globl lseek
lseek:
  mov $__NR_lseek, %rax
  syscall
  ret

// void exit(int code)
.globl exit
exit:
  mov $__NR_exit, %rax
  syscall
  ret

// void *mremap(void *old_address, size_t old_size,
//                    size_t new_size, int flags, void *new_address)
.globl mremap
mremap:
  mov $__NR_mremap, %rax
  mov %rcx, %r10
  syscall
  ret

// void *sbrk(int len)
.globl sbrk
sbrk:
  push %rdi
  xor %rdi, %rdi
  mov $__NR_brk, %rax
  syscall
  pop %rdi
  push %rax
  add %rax, %rdi
  mov $__NR_brk, %rax
  syscall
  pop %rax
  ret

