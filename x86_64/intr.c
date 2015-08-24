#include <intr.h>
#include <timer.h>
#include <kbd.h>
#include <pagefault.h>
#include <stdint.h>
#include <syscall.h>

#include <debug.h>

#define PIC1 0x20
#define PIC2 0xA0

#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)

#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define  ICW1_ICW4  0x01    /* ICW4 (not) needed */
#define  ICW1_SINGLE  0x02    /* Single (cascade) mode */
#define  ICW1_INTERVAL4  0x04    /* Call address interval 4 (8) */
#define  ICW1_LEVEL  0x08    /* Level triggered (edge) mode */
#define  ICW1_INIT  0x10    /* Initialization - required! */

#define  ICW4_8086  0x01    /* 8086/88 (MCS-80/85) mode */
#define  ICW4_AUTO  0x02    /* Auto (normal) EOI */
#define  ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define  ICW4_BUF_MASTER  0x0C    /* Buffered mode/master */
#define  ICW4_SFNM  0x10    /* Special fully nested (not) */

uint64_t IDT[256*2];

char *IDT_addr;
char *IDT_reg;

#define SYS_CODE_SELECTOR 0x8
#define SYS_IST 1 // Стек для прерываний
#define PAGE_IST 2 // И отдельный для страничной ошибки
#define CALL_IST 3 // И еще отдельный для системных вызовов

/*
 * Функция intr_install() устанавливает в качестве обработчика vector функцию func. 
 * Тип шлюза (прерывания [0x8e] или ловушки [0x8f]) указывается параметром type.
 * Фактически, эта функция создает (или изменяет) соответствующий дескриптор в таблице IDT
 */

void intr_install(uint8_t vector, void (*func)(), uint8_t type, uint8_t ist, uint16_t selector)
{
   uint8_t i;
   uint8_t b[16];

  b[0] =  (uint64_t)func & 0x00000000000000FF;  // Младшие два
  b[1] = ((uint64_t)func & 0x000000000000FF00) >> 8;  // байта адреса функции
  b[2] = selector&0xFF;  // Селектор сегмента
  b[3] = selector>>8;  // кода
  b[4] = ist&0x07;  // Поле Interrupt Stack Table
  b[5] = type&0xFF;  // Тип функции-обработчика
  b[6] = ( (uint64_t)func & 0x0000000000FF0000) >> 16;  // Средние
  b[7] = ( (uint64_t)func & 0x00000000FF000000) >> 24;  // два байта
  
  // Специфично для 64-бит
  b[8] = ( (uint64_t)func & 0x000000FF00000000) >> 32;  // Старшие четыре байта
  b[9] = ( (uint64_t)func & 0x0000FF0000000000) >> 40;
  b[10] = ( (uint64_t)func & 0x00FF000000000000) >> 48;
  b[11] = ( (uint64_t)func & 0xFF00000000000000) >> 56;
  
  b[12] = 0x00;  // Зарезервировано
  b[13] = 0x00;
  b[14] = 0x00;
  b[15] = 0x00;
  
  for(i = 0; i < 16; i++)
  {
    IDT_addr[vector*16 + i] = b[i];
  }
}

/*
 * Устанавливает системное прерывание
 */
inline void s_intr_install(uint8_t vector, void (*func)(), uint8_t type)
{
  intr_install(vector, func, type, SYS_IST, SYS_CODE_SELECTOR);
}

/*
 * Экспортируется в другие модули для установки своих
 * обработчиков прерываний
 */
void ext_intr_install(uint8_t vector, void (*func)())
{
  s_intr_install(vector, func, INTR_PRESENT|INTR_INTR_GATE);
}

void IDT_load()
{
  asm("lidt 0(,%0,)"::"a"((uint64_t)IDT_reg));
}

void intr_setup()
{
  IDT_reg = (char *)((uint64_t)IDT_addr + 256*16);
  uint16_t *table_limit;
  uint64_t *table_address;
  table_limit = (uint16_t *)IDT_reg;
  table_address = (uint64_t *)(((uint64_t)IDT_reg) + 2); 

  *table_limit = 256*16 - 1;
  *table_address = (uint64_t)IDT_addr;

  // Установим вектор прерываний в 0x20
  outb(PIC1_COMMAND, ICW1_INIT | ICW1_INTERVAL4 | ICW1_ICW4); // Инициализация
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_INTERVAL4 | ICW1_ICW4);

  outb(PIC1_DATA, 0x20);  // Вот он вектор для первого контроллера
  outb(PIC2_DATA, 0x28);  // Второго

  outb(PIC1_DATA, 0x04);  // 00000100 = 0x4
  outb(PIC2_DATA, 0x02);  // 00000010

  outb(PIC1_DATA, ICW4_8086);
  outb(PIC2_DATA, ICW4_8086);

  IDT_load();
}

/* 
 * Разрешает указанное прерывание
 * 0-7 - прерывания первого контроллера
 * 8-15 - прерывания второго контроллера
 */
void unmask_irq(uint8_t irq)
{
  if(irq > 15) return;
  char mask = inb(0x21 + 8*(irq>>3));
  mask = mask&(~(1<<(irq&7)));
  outb(0x21 + 8*(irq>>3), mask); 
}

/*
 * Запрещает прерывание
 */
void mask_irq(uint8_t irq)
{
  if(irq > 15) return;
  char mask = inb(0x21 + 8*(irq>>3));
  mask = mask|(1<<(irq&7));
  outb(0x21 + 8*(irq>>3), mask); 
}

uint8_t intr_sema = 0;

void intr_enable()
{
  if (intr_sema == 0)
  {
    kprintf("Bug: intr_enable!\n");
    BREAK();
    asm("hlt");
  }
  if (--intr_sema == 0)
    asm("sti");
}

void intr_disable()
{
  intr_sema++;
  asm("cli");
}

void intr_init()
{
  intr_disable();
  IDT_addr = (char *)(&IDT[0]);
  s_intr_install(0x20, &timer_intr, INTR_PRESENT|INTR_INTR_GATE);
  s_intr_install(0x21, &kbd_intr, INTR_PRESENT|INTR_INTR_GATE);
  //s_intr_install(0x0E, &page_fault, INTR_PRESENT|INTR_INTR_GATE);
  intr_install(0x0E, &page_fault, INTR_PRESENT|INTR_INTR_GATE, PAGE_IST, SYS_CODE_SELECTOR);
  //s_intr_install(0x80, &syscall_handler, INTR_PRESENT|INTR_INTR_GATE);
  intr_install(0x80, &syscall_handler, INTR_PRESENT|INTR_INTR_GATE|INTR_DPL3, CALL_IST, SYS_CODE_SELECTOR);
  intr_setup();
  mask_irq(0); // отключим PIT - мы будем использовать APIC timer
  intr_enable();
}

void intr_init_ap()
{
  IDT_reg = (char *)((uint64_t)IDT_addr + 256*16);
  uint16_t *table_limit;
  uint64_t *table_address;
  table_limit = (uint16_t *)IDT_reg;
  table_address = (uint64_t *)(((uint64_t)IDT_reg) + 2); 

  *table_limit = 256*16 - 1;
  *table_address = (uint64_t)IDT_addr;

  IDT_load();
}
