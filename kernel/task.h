#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include <regs.h>

#include <smp.h>
#include <apic.h>
#include <tls.h>

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
  all_regs r;
  uint64_t cr3;
  struct _task *next;
  // Состояние задачи
  uint8_t state;
  // Код возврата задачи
  int32_t status;
  thread_ls *tls;
} task;

#pragma pack()

/*
 * Состояния, в которых может находиться задача
 */
enum
{
  TASK_ACTIVE = 0,
  TASK_RUNNING,
  TASK_STARTING,
};

void tss_init();

void tss_init_cpu(uint8_t id);

void task_init(void);

void task_init_cpu(uint8_t id);

uint64_t task_switch(all_regs *r);

int task_fork();

uint64_t change_stack();

// Массив указателей на текущие задачи для каждого процессора
extern volatile task *curr[MAX_CPU_NR];
// Указатель на текущую задачу для текущего процессора
#define curr_cpu_task (curr[apic_get_id()])

#endif  //__TASK_H_
