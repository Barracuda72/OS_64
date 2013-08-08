#ifndef _TASK_H_
#define _TASK_H_

//  64-битная TSS
//  Собственно, тут ничего нет, за
//  исключением адресов стеков
#pragma pack(1)
typedef struct _TSS64
{
  int r1;    //Зарезервировано
  unsigned long rsp0;
  unsigned long rsp1;
  unsigned long rsp2;
  unsigned long r2;  //Зарезервировано
  unsigned long ist1;
  unsigned long ist2;
  unsigned long ist3;
  unsigned long ist4;
  unsigned long ist5;
  unsigned long ist6;
  unsigned long ist7;
  unsigned long r3;  //Зарезервировано
  short r4;    //Зарезервировано
  unsigned short IOMapAddr;  
} TSS64;

/*
 * Структура, описывающая задачу
 */
typedef struct _task
{
  unsigned int pid;
  unsigned long rip;
  unsigned long rsp;
  unsigned long rbp;
  unsigned long cr3;
  struct _task *next;
} task;
#pragma pack()

void tss_init();

void task_init(void);

void task_switch();

void change_stack();
#endif  //__TASK_H_
