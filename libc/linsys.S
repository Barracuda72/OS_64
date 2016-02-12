/*
 * linsys.S
 *
 * Обертка системных вызовов Linux 
 * для hosted-режима
 *
 */

#include "/usr/include/asm/unistd_64.h"

// Общий макрос описания системного вызова
#define SYSCALL(name, num) \
  .globl #name; \
  ##name: ; \
  mov $##num, %rax; \
  syscall; \
  ret


// ssize_t write(int fd, const void *buf, size_t count)
SYSCALL(write, __NR_write)

// ssize_t read(int fd, void *buf, size_t count)
SYSCALL(read, __NR_read)

// int open(const char *pathname, int flags)
SYSCALL(open, __NR_open)

// int close(int fd)
SYSCALL(close, __NR_close)

// int creat(const char *pathname, int flags)
SYSCALL(creat, __NR_creat)

// off_t lseek(int fd, off_t offset, int whence)
SYSCALL(lseek, __NR_lseek)

// void exit(int code)
SYSCALL(exit, __NR_exit)

// int rename(const char *oldpath, const char *newpath)
SYSCALL(rename, __NR_rename)

// int unlink(const char *pathname);
SYSCALL(unlink, __NR_unlink)

// int getpid(void)
SYSCALL(getpid, __NR_getpid)

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


