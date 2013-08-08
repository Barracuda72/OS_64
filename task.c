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

void task_init()
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

