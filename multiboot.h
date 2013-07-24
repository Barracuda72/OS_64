#ifndef __MULTIBOOT_H
#define __MULTIBOOT_H

/* Сигнатура заголовка Multiboot. */
#define MULTIBOOT_HEADER_MAGIC	0x1BADB002
#define MULTIBOOT_LOADER_MAGIC	0x2BADB002

/* Флаги для заголовка Multiboot. */
#define MULTIBOOT_FLAG_ALIGN	0x00000001	//Выравнивание ядра и модулей по границе страницы
#define MULTIBOOT_FLAG_MINFO	0x00000002	//Загрузчик обязан предоставить информацию о памяти
#define MULTIBOOT_FLAG_VINFO	0x00000004	//Загрузчик должен передать информацию о видеорежимах

// Итоговые флаги
#define MULTIBOOT_HEADER_FLAGS         (MULTIBOOT_FLAG_ALIGN | MULTIBOOT_FLAG_MINFO)

//Контрольная сумма
#define MULTIBOOT_HEADER_CHKSM	-(MULTIBOOT_HEADER_FLAGS + MULTIBOOT_HEADER_MAGIC)


/* Размер страницы */
#define PAGE_SIZE			0x1000

/* Размер GDT */
#define	GDT_SIZE	PAGE_SIZE

/* Размер стека кторый мы будем использовать. */
#define STACK_SIZE                      (PAGE_SIZE*4)

#ifndef __ASSEMBLY__
/* Этот код будет влючен только в C-файлы */ 
/* Заголовок таблицы секций ELF. */
typedef struct elf_section_header_table
{
 	unsigned long num;
 	unsigned long size;
 	unsigned long addr;
 	unsigned long shndx;
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
  	unsigned long flags8;
  	unsigned long mem_lower;
  	unsigned long mem_upper;
  	unsigned long boot_device;
  	unsigned long cmdline;
  	unsigned long mods_count;
  	unsigned long mods_addr;
  	elf_section_header_table_t elf_sec;
  	unsigned long mmap_length;
  	unsigned long mmap_addr;
} multiboot_info_t;
  
/* Структура, описывающая модуль */
typedef struct module_info
{
	unsigned long mod_start;
	unsigned long mod_end;
	unsigned long string;
	unsigned long reserved;
} module_info_t;

typedef struct mmap_info
{
	unsigned long size;
	unsigned long base_addr_low;
	unsigned long base_addr_high;
	unsigned long length_low;
	unsigned long length_high;
	unsigned long type;
} mmap_info_t;

#endif /*!__ASSEMBLY__ */
   
#endif /*__MULTIBOOT_H */

