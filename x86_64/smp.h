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
typedef struct __attribute((packed))
{
  uint32_t magic;    // "_MP_"
  uint32_t config;    // Указатель на структуру, описывающую систему
  uint8_t lenght;    // Длина структуры в 16-байтовых блоках. Всегда 1
  uint8_t version;    // Версия спецификации
  uint8_t chksum;    // Контрольная сумма
  uint8_t features[5];  // Поддерживаемые возможности
} SMP;

// Структура конфигурации
typedef struct __attribute((packed))
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
  uint8_t reserved;
} SMP_config;

// Описание процессора
typedef struct __attribute((packed))
{
  uint8_t type;    // Всегда 0 - это процессор
  uint8_t apic_id;    // Local APIC ID
  uint8_t apic_version;  // Версия Local APIC
  uint8_t enabled : 1;  // 1 бит - процессор включен
  uint8_t bsp : 1;    // Это Bootstrap Processor?
  uint8_t reserved : 6;  // Оставшиеся 6 бит
  uint32_t cpu_sig;    // Сигнатура процессора (как по CPUID)
  uint32_t cpu_features;  // Возможности процессора (как по CPUID)
  uint32_t reserved2;    // Дополним до 20 байт
} SMP_proc;

// Описание шины
typedef struct __attribute((packed))
{
  uint8_t type; // 1 - Шина
  uint8_t id;
  uint8_t name[6];
} SMP_bus;

// Описание IO APIC
typedef struct __attribute((packed))
{
  uint8_t type; // 2 - IO APIC
  uint8_t apic_id;
  uint8_t apic_version;
  uint8_t flags;
  uint32_t address;  
} SMP_ioapic;

// Описание источника прерываний
typedef struct __attribute((packed))
{
  uint8_t type; // 3 - источник прерываний
  uint8_t irq_type;
  uint16_t irq_flag;
  uint8_t src_bus;
  uint8_t src_bus_irq;
  uint8_t dst_apic;
  uint8_t dst_irq;
} SMP_intrsrc;

enum SMP_ENT
{
  SMP_PROCESSOR = 0,
  SMP_BUS,
  SMP_IOAPIC,
  SMP_INTRSRC
};

// Инициализация SMP
void smp_init(void);

// Флаг того, что BSP уже запущен
extern uint8_t bsp_started;

// Количество процессоров в системе
extern uint8_t cpu_nr;

// Максимальное количество процессоров
#define MAX_CPU_NR 16
#endif

