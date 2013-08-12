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
#define MULTIBOOT_HEADER_FLAGS         (MULTIBOOT_FLAG_ALIGN | MULTIBOOT_FLAG_MINFO)

// Контрольная сумма
#define MULTIBOOT_HEADER_CHKSM  -(MULTIBOOT_HEADER_FLAGS + MULTIBOOT_HEADER_MAGIC)


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
  unsigned int num;
  unsigned int size;
  unsigned int addr;
  unsigned int shndx;
} elf_section_header_table_t;

  
typedef struct {
  unsigned long   sh_name;
  unsigned long   sh_type;
  unsigned long   sh_flags;
  unsigned long   sh_addr;
  unsigned long   sh_offset;
  unsigned long   sh_size;
  unsigned long   sh_link;
  unsigned long   sh_info;
  unsigned long   sh_addralign;
  unsigned long   sh_entsize;
} Elf32_Shdr;

/* Информационная структура multiboot. */
typedef struct multiboot_info
{
  unsigned int flags8;
  unsigned int mem_lower;
  unsigned int mem_upper;
  unsigned int boot_device;
  unsigned int cmdline;
  unsigned int mods_count;
  unsigned int mods_addr;
  elf_section_header_table_t elf_sec;
  unsigned int mmap_length;
  unsigned int mmap_addr;
} multiboot_info_t;
  
/* Структура, описывающая модуль */
typedef struct module_info
{
  unsigned int mod_start;
  unsigned int mod_end;
  unsigned int string;
  unsigned int reserved;
} module_info_t;

#pragma pack(1)
typedef struct mmap_info
{
  unsigned int size;
  unsigned long base_addr;
  unsigned long length;
  unsigned int type;
} mmap_info_t __attribute__((packed));
#pragma pack()

#endif /*!__ASSEMBLY__ */
   
#endif /*__MULTIBOOT_H */

