#ifndef __CPUID_H__
#define __CPUID_H__

// Функции
char *CPU_get_vendor_string(void);
unsigned int CPU_get_info(void);
unsigned int CPU_get_xinfo(void);

// Макросы

// Семейство
#define CPU_get_info_family() ((CPU_get_info()&0xF00)>>8)

// Модель
#define CPU_get_info_model() ((CPU_get_info()&0xF0)>>4)

// Степпинг
#define CPU_get_info_stepping() (CPU_get_info()&0xF)

// Тип
#define CPU_get_info_type() ((CPU_get_info()&0x3000)>>12)
#endif //__CPUID_H__
