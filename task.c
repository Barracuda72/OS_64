#include <task.h>
#include <gdt.h>
#include <klibc.h>

#include <debug.h>

//Инициализируем многозадачность (громко сказано - просто устанавливаем стек для прерываний)

TSS64 IntrTss;

unsigned long intr_s[1024];
unsigned short s = 0;	// Селектор задачи ядра

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

void get_s(void)
{
  printf("Task %d running...\n", s);
}


