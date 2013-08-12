.text
.code64
.globl timer_intr

timer_intr:
  // В стеке прерывания лежат:
  // 0x00: %rip старой задачи
  // 0x08: %cs
  // 0x10: %rflags старой задачи
  // 0x18: %rsp старой задачи
  // 0x20: %ss
  //xchg %bx, %bx
  push %rdi
  // Заменяем стек на стек текущей задачи
  mov  0x20(%rsp), %rdi
  xchg %rdi, %rsp
  // %rdi = указатель на стек прерывания
  // Сохраняем регистры на стек текущей задачи
  push %rax // %rax
  push %rbx // %rbx
  push %rcx // %rcx
  push %rdx // %rdx
  push %rsi // %rsi
  push %rbp // %rbp
  mov  (%rdi), %rbx 
  push %rbx // %rdi
  mov  %cr3, %rbx
  push %rbx // %cr3
  // Выкидываем значение %rdi из стека прерывания
  add  $8, %rdi

  // Вызываем функцию на стеке прерывания
  // На данные в стеке это влияния не окажет
  mov  %rdi, %rsp
  call task_switch
  // %rax = стек новой задачи
  sub  $0x40, %rax
  mov  %rax, %rsp

  movb $0x20, %al
  outb %al, $0x20

  //xchg %bx, %bx
  // Восстанавливаем регистры новой задачи
  pop  %rbx
  mov  %rbx, %cr3
  mov  0x08(%rsp), %rbp
  mov  0x10(%rsp), %rsi
  mov  0x18(%rsp), %rdx
  mov  0x20(%rsp), %rcx
  mov  0x28(%rsp), %rbx
  mov  0x30(%rsp), %rax
  // Меняем стеки обратно
  xchg %rsp, %rdi
  mov  (%rdi), %rdi
  iretq
