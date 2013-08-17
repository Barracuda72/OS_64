#ifndef __REGS_H__
#define __REGS_H__

#include <stdint.h>
// Здесь будут разные хитрые штуки с регистрами
// чтоб не дублировать в разных файлах

// Регистры, лежащие в стеке при любом прерывании
typedef struct
{
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
} intr_regs;

// Общие регистры, которые будет сохранять
// сам обработчик прерывания
typedef struct
{
  uint64_t r9;
  uint64_t r8;
  uint64_t rbp;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rbx;
  uint64_t rax;
} comm_regs;

// Обработчик страничного прерывания ожидает 
// код ошибки, переключателю задач негде 
// хранить cr3... выход - есть!
typedef struct
{
  comm_regs c;
  uint64_t reg1; // Одного регистра должно хватитть всем!
  intr_regs i;
} all_regs;

#endif // __REGS_H__
