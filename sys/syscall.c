#include <syscall.h>
#include <stdint.h>
#include <kprintf.h>

void test_out(char *, uint64_t);

void *syscall_fs[] = 
{ 
 &test_out,
 &sys_exit,
 &sys_close,
 &sys_execve,
 &sys_fork,
 &sys_fstat,
 &sys_getpid,
 &sys_isatty,
 &sys_kill,
 &sys_link,
 &sys_lseek,
 &sys_open,
 &sys_read,
 &sys_sbrk,
 &sys_stat,
 &sys_times,
 &sys_unlink,
 &sys_wait,
 &sys_write,
};

uint64_t syscall_ns = 19;

asm(".globl syscall_handler \n \
syscall_handler: \n \
  # RAX = номер системного вызова \n \
  # Параметры, как обычно - rdi, rsi, rdx, rcx, r8, r9 \n \
  push %rbx \n \
  mov $syscall_ns, %rbx \n \
  mov (%rbx), %rbx \n \
  cmp %rax, %rbx \n \
  jle .ret \n \
  mov $syscall_fs, %rbx \n \
  shl $3, %rax \n \
  add %rbx, %rax \n \
  mov (%rax), %rbx \n \
  call save_regs \n\
  call *%rbx \n \
  # Подменяем возвращаемое значение на вершине стека \n \
  xchg (%rsp), %rax \n \
  call rest_regs \n \
.ret: \n \
  pop %rbx \n \
  iretq \n \
");

void test_out(char *s, uint64_t t)
{
  kprintf(s, t);
}
