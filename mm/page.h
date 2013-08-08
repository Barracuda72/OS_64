#ifndef __PAGE_H__
#define __PAGE_H__

#include <intr.h>

struct _pde_pte_desc  //Структура, описывающая дескриптор в PDT и PT
{                                                                                        
  unsigned long exists : 1;  // Страница присутствует в памяти
  unsigned long writable : 1;  // Страница доступна для записи
  unsigned long r3_access : 1;  // Страница доступна пользователю (уровень привелегий 3)
  unsigned long cache_mode : 1;  // Режим кеширования страницы (write back (0) / write through (1))
  unsigned long cache_enable : 1;  // Кешировние разрешено/запрещено
  unsigned long wasaccess : 1;  // К странице было обращение
  unsigned long waswritten : 1;  // В страницу была произведена запись
  unsigned long bigpage : 1;  // "Большая" страница
  unsigned long globalpage : 1;  // Глобальная страница - не выгружается из кеша TLB при перезагрузке CR3 (PDBR)
  unsigned long reserved : 3;  // Зарезервировано для ОС
  unsigned long address : 40;  // Физический адрес страницы
  unsigned long avail : 11;  // Свободно
  unsigned long exd : 1;    // Запретить исполнение страницы
} __attribute((packed));

struct _linear  // Структура, представляющая линейный адрес страницы
{
  unsigned long offset : 12;  // Смещение в странице
  unsigned long pt : 9;
  unsigned long pd : 9;
  unsigned long pdp : 9;
  unsigned long pml4 : 9;
  unsigned long reserved : 16;
} __attribute((packed));

typedef union
{
  unsigned long h;
  struct _linear l;
} linear;

typedef union
{
  unsigned long h;  // Шестнадцатиричное представление pte (pde)
  struct _pde_pte_desc p;  // Наглядное представление
} pte_desc;

void page_init(unsigned long *last);

void mount_page(void *physical, void *logical);
void umount_page(void *logical);
void *alloc_page();
void free_page(void *p);
void *copy_pages();

#endif //__PAGE_H__
