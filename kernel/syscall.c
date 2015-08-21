#include <syscall.h>
#include <stdint.h>

void test_out(char *, uint64_t);

void *syscall_fs[1] = 
{ 
 &test_out, 
};

uint64_t syscall_ns = 1;

asm(".globl syscall_handler \n \
syscall_handler: \n \
  # RAX = номер системного вызова \n \
  # Параметры, как обычно - rdi, rsi, rdx, rcx, r8, r9 \n \
  # xchg %bx, %bx \n \
  push %rbx \n \
  mov $syscall_ns, %rbx \n \
  mov (%rbx), %rbx \n \
  cmp %rax, %rbx \n \
  jle .ret \n \
  mov $syscall_fs, %rbx \n \
  shl $3, %rax \n \
  add %rbx, %rax \n \
  mov (%rax), %rbx \n \
  call *%rbx \n \
.ret: \n \
  pop %rbx \n \
  iretq \n \
");

void test_out(char *s, uint64_t t)
{
  kprintf(s, t);
}

// И - сам вызов
DEFN_SYSCALL2(test_out, 0, const char *, unsigned long)
