#ifndef __INTR_H__
#define __INTR_H__

#include <stdint.h>

void intr_install(uint8_t vector, void (*func)(), uint8_t type, uint8_t pc, uint16_t selec);

void ext_intr_install(uint8_t vector, void (*func)());

void intr_init();

void intr_init_ap();

void intr_setup();

void intr_enable();

void intr_disable();

/*
 * Макрос-заготовка для обработчика прерываний
 */
#define IRQ_HANDLER(func) void func (void);\
 asm(#func ": \n \
 push %rax \n \
 push %rbx \n \
 push %rcx \n \
 push %rdx \n \
 push %rsi \n \
 push %rdi \n \
 push %rbp \n \
 call _" #func " \n \
" #func "_end: \n \
 movb $0x20, %al \n \
 outb %al, $0x20 \n \
 pop %rbp \n \
 pop %rdi \n \
 pop %rsi \n \
 pop %rdx \n \
 pop %rcx \n \
 pop %rbx \n \
 pop %rax \n \
 iretq \n \
 .globl " #func "_end \n \
");\
 void _ ## func(void)

#define INTR_CALL_GATE  0x0C
//#define INTR_TASK_GATE  0x05  //Не используется в 64-бит режиме
#define INTR_INTR_GATE  0x0E
#define INTR_TRAP_GATE  0x0F
#define INTR_PRESENT  0x80

#define INTR_DPL0  0x00
#define INTR_DPL1  0x20
#define INTR_DPL2  0x40
#define INTR_DPL3  0x60
//#define INTR_NOTSYS  0x10  //Не используется в 64-разрядном режиме

#endif //__INTR_H__
