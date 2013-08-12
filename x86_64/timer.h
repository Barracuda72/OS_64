#ifndef __TIMER_H__
#define __TIMER_H__

#include <intr.h>
#include <ktty.h>
#include <task.h>

#include <debug.h>

/*
 * Обработчик прерывания таймера
 */
/*
IRQ_HANDLER(timer_intr)
{
  //Пока пуст
  //asm("push %rax");
  //ktty_putc('T');
  task_switch();
}*/

void timer_intr();

#endif //__TIMER_H__
