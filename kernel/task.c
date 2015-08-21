#include <task.h>
#include <gdt.h>
#include <kprintf.h>
#include <mem.h>
#include <multiboot.h>
#include <stdint.h>

#include <debug.h>

/*
 * Инициализируем многозадачность (громко сказано - просто 
 * устанавливаем стек для прерываний)
 */

TSS64 IntrTss;

// Стек для прерываний
uint64_t intr_s[1024];
// Стек для страничной ошибки
uint64_t fault_s[1024];
// Стек для системных вызовов
uint64_t call_s[1024];
uint16_t s = 0;  // Селектор задачи ядра
volatile task *curr = 0; // Текущая выполняемая задача
uint64_t next_pid = 1;

void tss_init()
{
  IntrTss.rsp0 = (uint64_t)&intr_s[1022];
  IntrTss.rsp1 = (uint64_t)&intr_s[1022];
  IntrTss.rsp2 = (uint64_t)&intr_s[1022];
  
  IntrTss.ist1 = (uint64_t)&intr_s[1022];
  IntrTss.ist2 = (uint64_t)&fault_s[1022];
  IntrTss.ist3 = (uint64_t)&call_s[1022];
  IntrTss.ist4 = (uint64_t)&intr_s[1022];
  IntrTss.ist5 = (uint64_t)&intr_s[1022];
  IntrTss.ist6 = (uint64_t)&intr_s[1022];
  IntrTss.ist7 = (uint64_t)&intr_s[1022];
  
  s = GDT_smartaput(&IntrTss, sizeof(TSS64), SEG_PRESENT | SEG_TSS64 | SEG_DPL3);
  s = CALC_SELECTOR(s, SEG_GDT | SEG_RPL3);
  
  //BREAK();
  asm("ltr s");
}

void task_init()
{
  intr_disable();
  curr = kmalloc(sizeof(task));
  curr->pid = next_pid++;
  zeromem(&(curr->r), sizeof(all_regs));
  curr->next = curr; // Закольцовываем
  intr_enable();
}

/*
 * Создает новый стек и переключается на него
 */
uint64_t change_stack()
{
  extern uint64_t stack;
  uint64_t stack_old = &stack;
  uint64_t stack_end = (uint64_t) stack_old + STACK_SIZE;
  uint64_t *new_stack = kmalloc(STACK_SIZE);
  memcpy(new_stack, stack_old, STACK_SIZE);
  uint64_t offset = (uint64_t)new_stack - (uint64_t)stack_old;
  uint64_t i;
  for (i = 0; i < STACK_SIZE; i++)
  {
    if ((new_stack[i] >= stack_end) && 
        (new_stack[i] <= (uint64_t)stack_old))
    {
      new_stack[i] += offset;
    }
  } 

  // Перемещаем стек
  /*asm volatile("add %0, %%rsp\n \
                add %0, %%rbp\n" :: "r" (offset)); */
  //BREAK();
  return offset;
}

/*
 * Функция подготавливает стек для задачи таким образом,
 * чтобы все выглядело, будто задача была прервана.
 * Ну и за одним возвращает адрес возврата.
 */
uint64_t read_rip(uint64_t cr3, uint64_t rsp_off, all_regs *r);
asm("\n \
read_rip_old: \n \
  movq %rsp, %rax \n \
  addq (%rsi), %rax \n \
  subq $0x48, %rax # 0x50 займут регистры, минус адрес возврата \n \
  movq %r8,  0x48(%rax) \n \
  movq %r9,  0x40(%rax) \n \
  movq $0,   0x38(%rax) # RAX дочерней задачи \n \
  movq %rbx, 0x30(%rax) \n \
  movq %rcx, 0x28(%rax) \n \
  movq %rdx, 0x20(%rax) \n \
  movq %rsi, 0x18(%rax) \n \
  push %rbp \n \
  addq (%rsi), %rbp \n \
  movq %rbp, 0x10(%rax) \n \
  pop  %rbp \n \
  movq %rdi, 0x08(%rax) \n \
  movq %rdi, 0x00(%rax) # CR3 дочерней задачи \n \
  \n \
  addq $0x50, %rax \n \
  movq %rax, (%rsi) # Адрес стека дочерней задачи \n \
  movq (%rsp), %rax # Будущий адрес возврата из прерывания \n \
  ret \n \
");

asm("\n \
read_rip: \n \
  mov %rsp, %rax \n \
  add %rsi, %rax # %rax = стек дочерней задачи\n \
  xchg %rbp, %rsi # Подменим и скорректируем rbp \n \
  add %rsi, %rbp \n \
  add $8, %rax # Выкинем адрес возврата \n \
  xchg %rdx, %rsp \n \
  \n \
  pushq $0x10 # SS \n \
  push %rax # RSP \n \
  pushfq \n \
  pop %rax \n \
  or $0x200, %rax # Разрешим дочерней задаче прерывания\n \
  push %rax # RFLAGS \n \
  pushq $0x08 # CS \n \
  mov (%rdx), %rax \n \
  push %rax # RIP \n \
  \n \
  push %rdi # CR3 дочерней задачи \n \
  mov $0, %rax # RAX дочерней задачи \n \
  call save_regs \n \
  xchg %rdx, %rsp \n \
  xchg %rbp, %rsi \n \
  mov (%rsp), %rax \n \
  ret \n \
");

int task_fork()
{
  uint64_t rip, off, cr3;
  intr_disable();
  
  off = change_stack();
  cr3 = copy_pages();

  // Родительская задача - готовим все
  task *new = kmalloc(sizeof(task));
  new->pid = next_pid++;
  //BREAK();
  rip = read_rip(cr3, off, (((uint64_t)&(new->r)) + sizeof(all_regs)));
  if (rip == 0)
    return 0; // Дочерняя задача

  new->next = curr->next;
  curr->next = new;
  intr_enable();
  return new->pid;
}

uint64_t task_switch(all_regs *r)
{
  if (curr != 0)
  {
    memcpy(&(curr->r), r, sizeof(all_regs));
    curr = curr->next;
    // Стек, с которого будем восстанавливать регистры
    return &(curr->r);
  }
  return r;
}

void get_c()
{
  kprintf("Curr: %l\n", curr);
}
