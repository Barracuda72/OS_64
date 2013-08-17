.text
.code64

// Сохраняет регистры на текущий стек
.globl save_regs
save_regs:
  xchg (%rsp), %rax
  push %rbx
  push %rcx
  push %rdx
  push %rsi
  push %rdi
  push %rbp
  push %r8
  push %r9
  jmp *%rax

// Восстанавливает регистры
.globl rest_regs
rest_regs:
  pop %rax
  xchg 0x40(%rsp), %rax
  pop %r9
  pop %r8
  pop %rbp
  pop %rdi
  pop %rsi
  pop %rdx
  pop %rcx
  pop %rbx
  ret
