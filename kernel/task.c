#include <task.h>
#include <gdt.h>
#include <klibc.h>
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
  IntrTss.ist3 = (uint64_t)&intr_s[1022];
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
  curr = kmalloc(sizeof(task));
  curr->pid = next_pid++;
  curr->rsp = 0;
  //curr->rbp = 0;
  curr->rflags = 0;
  curr->rip = 0;
  curr->cr3 = 0;
  curr->next = curr; // Закольцовываем
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
 * Своровано из учебника James'M
 * Позже перепишу более красиво
 */
uint64_t read_rip_old();
asm("\n \
read_rip_old: \n \
  pop %rax \n \
  jmp *%rax \n \
");

/*
 * Функция подготавливает стек для задачи таким образом,
 * чтобы все выглядело, будто задача была прервана.
 * Ну и за одним возвращает адрес возврата.
 */
uint64_t read_rip(uint64_t cr3, uint64_t *rsp_off);
asm("\n \
read_rip: \n \
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

int task_fork()
{
  uint64_t rip, off, cr3, rflags;
  intr_disable();
  
  off = change_stack();
  cr3 = copy_pages();
  asm ("pushfq \n \
        pop %0":"=r"(rflags));
  rip = read_rip(cr3, &off);
  if (rip == 0)
    return 0; // Дочерняя задача

  // Родительская задача - готовим все
  task *new = kmalloc(sizeof(task));
  new->pid = next_pid++;
  new->rsp = off;
  new->rip = rip;  
  new->rflags = rflags|(1<<9); // Разрешаем дочерней задаче прерывания
  new->cr3 = cr3; // Вроде уже и не нужно

  new->next = curr->next;
  curr->next = new;
  intr_enable();
  return new->pid;
}

uint64_t task_switch(task_regs *r)
{
  // В r - стек прерывания
  if (curr != 0)
  {
    curr->rip = r->rip;
    curr->rflags = r->rflags;
    curr->rsp = r->rsp;

    curr = curr->next;

    r->rip = curr->rip;
    r->rflags = curr->rflags;
    r->rsp = curr->rsp;
  }
  return r->rsp;
}

void get_c()
{
  printf("Curr: %l\n", curr);
}
