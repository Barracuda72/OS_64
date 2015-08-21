#ifndef __MULTIBOOT_H
#define __MULTIBOOT_H

/* Сигнатура заголовка Multiboot. */
#define MULTIBOOT_HEADER_MAGIC  0x1BADB002
#define MULTIBOOT_LOADER_MAGIC  0x2BADB002

/* Флаги для заголовка Multiboot. */
#define MULTIBOOT_FLAG_ALIGN  0x00000001  // Выравнивание ядра и модулей по границе страницы
#define MULTIBOOT_FLAG_MINFO  0x00000002  // Загрузчик обязан предоставить информацию о памяти
#define MULTIBOOT_FLAG_VINFO  0x00000004  // Загрузчик должен передать информацию о видеорежимах

// Итоговые флаги
//#define MULTIBOOT_HEADER_FLAGS         (MULTIBOOT_FLAG_ALIGN | MULTIBOOT_FLAG_MINFO | MULTIBOOT_FLAG_VINFO)
#define MULTIBOOT_HEADER_FLAGS         (MULTIBOOT_FLAG_ALIGN | MULTIBOOT_FLAG_MINFO)

// Контрольная сумма
#define MULTIBOOT_HEADER_CHKSM  -(MULTIBOOT_HEADER_FLAGS + MULTIBOOT_HEADER_MAGIC)

// Параметры видеорежима
// 800x600@16
#define MULTIBOOT_VMODE_WIDTH  800
#define MULTIBOOT_VMODE_HEIGHT 600
#define MULTIBOOT_VMODE_DEPTH  16
// Тип указать нельзя, Syslinux его проверяет
#define MULTIBOOT_VMODE_TYPE   0

// Флаг, указывающий на включенный видеорежим
// и наличие информации о нем от загрузчика
#define MULTIBOOT_INFO_VINFO 0x800

/* Размер страницы */
#define PAGE_SIZE      0x1000

/* Размер GDT */
#define  GDT_SIZE  PAGE_SIZE

/* Размер стека кторый мы будем использовать. */
#define STACK_SIZE                      (PAGE_SIZE*4)

#ifndef __ASSEMBLY__
/* Этот код будет влючен только в C-файлы */ 
/* Заголовок таблицы секций ELF. */
typedef struct elf_section_header_table
{
  uint32_t num;
  uint32_t size;
  uint32_t addr;
  uint32_t shndx;
} elf_section_header_table_t;

  
typedef struct {
  uint64_t   sh_name;
  uint64_t   sh_type;
  uint64_t   sh_flags;
  uint64_t   sh_addr;
  uint64_t   sh_offset;
  uint64_t   sh_size;
  uint64_t   sh_link;
  uint64_t   sh_info;
  uint64_t   sh_addralign;
  uint64_t   sh_entsize;
} Elf32_Shdr;

#pragma pack(1)
/* Информационная структура multiboot. */
typedef struct __attribute__ ((packed))
{
  uint32_t flags8;
  
  uint32_t mem_lower;
  uint32_t mem_upper;
  
  uint32_t boot_device;
  
  uint32_t cmdline;

  uint32_t mods_count;
  uint32_t mods_addr;
  
  elf_section_header_table_t elf_sec;

  uint32_t mmap_length;
  uint32_t mmap_addr;

  uint32_t drives_length;
  uint32_t drives_addr;

  uint32_t config_table;

  uint32_t boot_loader_name;

  uint32_t apm_table;

  uint32_t vbe_control_info;
  uint32_t vbe_mode_info;
  uint16_t vbe_mode;
  uint16_t vbe_interface_seg;
  uint16_t vbe_interface_off;
  uint16_t vbe_interface_len;
} multiboot_info_t;
#pragma pack()
  
/* Структура, описывающая модуль */
typedef struct module_info
{
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t string;
  uint32_t reserved;
} module_info_t;

#pragma pack(1)
typedef struct  __attribute__((packed))
{
  uint32_t size;
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
} mmap_info_t; 
#pragma pack()

#endif /*!__ASSEMBLY__ */
   
#endif /*__MULTIBOOT_H */

