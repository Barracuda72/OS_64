#include <task.h>
#include <gdt.h>
#include <klibc.h>
#include <mem.h>
#include <multiboot.h>

#include <debug.h>

/*
 * Инициализируем многозадачность (громко сказано - просто 
 * устанавливаем стек для прерываний)
 */

TSS64 IntrTss;

unsigned long intr_s[1024];
unsigned short s = 0;  // Селектор задачи ядра
volatile task *curr = 0; // Текущая выполняемая задача
unsigned long next_pid = 1;

void tss_init()
{
  IntrTss.rsp0 = (unsigned long)&intr_s[1022];
  IntrTss.rsp1 = (unsigned long)&intr_s[1022];
  IntrTss.rsp2 = (unsigned long)&intr_s[1022];
  
  IntrTss.ist1 = (unsigned long)&intr_s[1022];
  IntrTss.ist2 = (unsigned long)&intr_s[1022];
  IntrTss.ist3 = (unsigned long)&intr_s[1022];
  IntrTss.ist4 = (unsigned long)&intr_s[1022];
  IntrTss.ist5 = (unsigned long)&intr_s[1022];
  IntrTss.ist6 = (unsigned long)&intr_s[1022];
  IntrTss.ist7 = (unsigned long)&intr_s[1022];
  
  s = GDT_smartaput(&IntrTss, sizeof(TSS64), SEG_PRESENT | SEG_TSS64);
  s = CALC_SELECTOR(s, SEG_GDT | SEG_RPL0);
  
  //BREAK();
  asm("ltr s");
}

void task_init()
{
  curr = kmalloc(sizeof(task));
  curr->pid = next_pid++;
  curr->rsp = 0;
  curr->rbp = 0;
  curr->rip = 0;
  curr->cr3 = 0;
  curr->next = curr; // Закольцовываем
}

/*
 * Создает новый стек и переключается на него
 */
void change_stack()
{
  extern unsigned long stack;
  unsigned long stack_old = &stack;
  unsigned long stack_end = (unsigned long) stack_old + STACK_SIZE;
  unsigned long *new_stack = kmalloc(STACK_SIZE);
  memcpy(new_stack, stack_old, STACK_SIZE);
  unsigned long offset = (unsigned long)new_stack - (unsigned long)stack_old;
  unsigned long i;
  for (i = 0; i < STACK_SIZE; i++)
  {
    if ((new_stack[i] >= stack_end) && 
        (new_stack[i] <= (unsigned long)stack_old))
    {
      new_stack[i] += offset;
    }
  } 

  // Перемещаем стек
  asm volatile("add %0, %%rsp\n \
                add %0, %%rbp\n" :: "r" (offset)); 
  //BREAK();
}

/*
 * Своровано из учебника James'M
 * Позже перепишу более красиво
 */
unsigned long read_rip();
asm("\n \
read_rip: \n \
  pop %rax \n \
  jmp *%rax \n \
");

void task_switch()
{
  if (curr == 0)
    return;
  ktty_putc('S');

  unsigned long rsp, rbp, rip, cr3;
  asm ("mov %%rsp, %0":"=r"(rsp));
  asm ("mov %%rbp, %0":"=r"(rbp));
  asm ("mov %%cr3, %0":"=r"(cr3));

  rip = read_rip();
  // Если мы только что переключили задачу
  if (rip == 0xDEADC0DEDEADBEEF)
    return;

  // Иначе - начнем переключение
  curr->rip = rip;
  curr->rsp = rsp;
  curr->rbp = rbp;
  curr->cr3 = cr3;

  curr = curr->next;

  rsp = curr->rsp;
  rbp = curr->rbp;
  cr3 = curr->cr3&0x000FFFFFFFFFF000; // Hack?
  
  asm volatile("         \
    cli;                 \
    mov %0, %%rcx;       \
    mov %1, %%rsp;       \
    mov %2, %%rbp;       \
    xchg %%bx, %%bx; \
    mov %3, %%cr3;       \
    mov $0xDEADC0DEDEADBEEF, %%rax; \
    sti;                 \
    jmp *%%rcx           "
  : : "r"(rip), "r"(rsp), "r"(rbp), "r"(cr3));
}

void get_c()
{
  printf("Curr: %l\n", curr);
}
