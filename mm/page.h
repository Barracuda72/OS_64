#ifndef __PAGE_H__
#define __PAGE_H__

#include <intr.h>
#include <stdint.h>

struct _pde_pte_desc  //Структура, описывающая дескриптор в PDT и PT
{                                                                                        
  uint64_t exists : 1;  // Страница присутствует в памяти
  uint64_t writable : 1;  // Страница доступна для записи
  uint64_t r3_access : 1;  // Страница доступна пользователю (уровень привелегий 3)
  uint64_t cache_mode : 1;  // Режим кеширования страницы (write back (0) / write through (1))
  uint64_t cache_disable : 1;  // Кешировние разрешено/запрещено
  uint64_t wasaccess : 1;  // К странице было обращение
  uint64_t waswritten : 1;  // В страницу была произведена запись
  uint64_t bigpage : 1;  // "Большая" страница
  uint64_t globalpage : 1;  // Глобальная страница - не выгружается из кеша TLB при перезагрузке CR3 (PDBR)
  uint64_t reserved : 3;  // Зарезервировано для ОС
  uint64_t address : 40;  // Физический адрес страницы
  uint64_t avail : 11;  // Свободно
  uint64_t exd : 1;    // Запретить исполнение страницы
} __attribute((packed));

struct _linear  // Структура, представляющая линейный адрес страницы
{
  uint64_t offset : 12;  // Смещение в странице
  uint64_t pt : 9;
  uint64_t pd : 9;
  uint64_t pdp : 9;
  uint64_t pml4 : 9;
  uint64_t reserved : 16;
} __attribute((packed));

typedef union
{
  uint64_t h;
  struct _linear l;
} linear;

typedef union
{
  uint64_t h;  // Шестнадцатиричное представление pte (pde)
  struct _pde_pte_desc p;  // Наглядное представление
} pte_desc;

void page_init(uint64_t *last);

void mount_page(void *physical, void *logical);
void mount_page_hw(void *physical, void *logical);
void *umount_page(void *logical);
void *alloc_page();
void free_page(void *p);
void *copy_pages();
void remap_pages(void *oldaddr, void *newaddr, uint64_t size);

#endif //__PAGE_H__
