/*
 * Symmetric Multiprocessing support
 * Как звучит-то, а? =)
*/
#ifndef __SMP_H__
#define __SMP_H__

#include <stdint.h>

// Идентификатор структуры, описывающей SMP
#define SMP_MAGIC 0x5F504D5F  // "_MP_"

// Идентификатор структуры, описывающей конфигурацию
#define SMP_CONFIG_MAGIC 0x504D4350  // "PCMP"

// Структура SMP
typedef struct
{
  uint32_t magic;    // "_MP_"
  uint32_t config;    // Указатель на структуру, описывающую систему
  uint8_t lenght;    // Длина структуры в 16-байтовых блоках. Всегда 1
  uint8_t version;    // Версия спецификации
  uint8_t chksum;    // Контрольная сумма
  uint8_t features[5];  // Поддерживаемые возможности
} SMP;

// Структура конфигурации
typedef struct
{
  uint32_t magic;    // "PCMP"
  uint16_t lenght;    // Размер структуры
  uint8_t revision;    // Ревизия спецификации
  uint8_t chksum;    // Контрольная сумма
  char oemid[8];      // Идентификатор производителя (материнской платы? процессора?)
  char productid[12];    // Идентификатор продукта (какого?)
  uint32_t oem_tbl;    // Указатель на таблицу OEM информации
  uint16_t oem_sz;    // Размер этой таблицы
  uint16_t count;    // Количество нижеследующих записей
  uint32_t lapic_addr;  // Адрес локального APIC
  uint16_t e_lenght;  // Размер расширенной таблицы
  uint8_t e_chksum;    // Контрольная сумма для расширенной таблицы, если таблицы нет - 0
} SMP_config __attribute((packed));

// Описание процессора
typedef struct
{
  uint8_t type;    // Всегда 0 - это процессор
  uint8_t lapic_id;    // Local APIC ID
  uint8_t lapic_version;  // Версия Local APIC
  uint8_t enabled : 1;  // 1 бит - процессор включен
  uint8_t bsp : 1;    // Это Bootstrap Processor?
  uint8_t reserved : 6;  // Оставшиеся 6 бит
  uint32_t cpu_sig;    // Сигнатура процессора (как по CPUID)
  uint32_t cpu_features;  // Возможности процессора (как по CPUID)
  uint32_t reserved2;    // Дополним до 20 байт
} SMP_proc __attribute((packed));

// Инициализация SMP
void smp_init(void);

/*
Local APIC Register Addresses 
Offset  Register Name  Software Read/Write
0x0000h - 0x0010  reserved  -
0x0020h  Local APIC ID Register Read/Write
0x0030h  Local APIC ID Version Register Read only
0x0040h - 0x0070h  reserved  -
0x0080h  Task Priority Register Read/Write
0x0090h  Arbitration Priority Register Read only
0x00A0h  Processor Priority Register Read only
0x00B0h  EOI Register  Write only
0x00C0h  reserved  -
0x00D0h  Logical Destination Register Read/Write
0x00E0h  Destination Format Register Bits 0-27 Read only, Bits 28-31 Read/Write
0x00F0h  Spurious-Interrupt Vector Register Bits 0-3 Read only, Bits 4-9 Read/Write
0x0100h - 0x0170  ISR 0-255 Read only
0x0180h - 0x01F0h  TMR 0-255 Read only
0x0200h - 0x0270h  IRR 0-255 Read only
0x0280h  Error Status Register Read only
0x0290h - 0x02F0h  reserved  -
0x0300h  Interrupt Command Register 0-31 Read/Write
0x0310h  Interrupt Command Register 32-63 Read/Write
0x0320h  Local Vector Table (Timer) Read/Write
0x0330h  reserved  -
0x0340h  Performance Counter LVT Read/Write
0x0350h  Local Vector Table (LINT0) Read/Write
0x0360h  Local Vector Table (LINT1) Read/Write
0x0370h  Local Vector Table (Error) Read/Write
0x0380h  Initial Count Register for Timer Read/Write
0x0390h  Current Count Register for Timer Read only
0x03A0h - 0x03D0h  reserved  -
0x03E0h  Timer Divide Configuration Register Read/Write
0x03F0h  reserved  -
*/
#endif

