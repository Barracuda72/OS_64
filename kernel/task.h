#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>

//  64-битная TSS
//  Собственно, тут ничего нет, за
//  исключением адресов стеков
#pragma pack(1)
typedef struct _TSS64
{
  int r1;    //Зарезервировано
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t r2;  //Зарезервировано
  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;
  uint64_t r3;  //Зарезервировано
  short r4;    //Зарезервировано
  uint16_t IOMapAddr;  
} TSS64;

/*
 * Структура, описывающая задачу
 */
typedef struct _task
{
  uint64_t pid;
  uint64_t rip;
  uint64_t rsp;
  uint64_t rflags;
  uint64_t cr3;
  struct _task *next;
} task;

typedef struct _tregs
{
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
} task_regs;
#pragma pack()

void tss_init();

void task_init(void);

uint64_t task_switch(task_regs *r);

int task_fork();
#endif  //__TASK_H_
