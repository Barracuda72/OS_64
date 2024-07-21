#include <mem.h>
#include <stdint.h>
#include <multiboot.h>

#include <phys.h>
#include <page.h>

#include <kprintf.h>

#include <debug.h>

kmem_header *kernel_heap;
uint64_t kernel_hlim;

void mem_init(uint64_t pl_addr, uint64_t pl_size, 
    multiboot_info_t *mb)
{
  // last_page - последняя страница как физической,
  // так и виртуальной памяти
  uint64_t last_page = (pl_addr&0x0FFFF000) + 0x1000;
  //kprintf("Last phys page is %l\n", last_page);
  page_init(&last_page);
  phys_init(&last_page, pl_size, mb);

  // Размер кучи ~1Gb
  kernel_hlim = 0x40000000 - last_page - sizeof(kmem_header);
  kernel_heap = (kmem_header *)(last_page|0xFFFFFFFFC0000000);
  kernel_heap->magic = KMEM_MAGIC;
  kernel_heap->size = kernel_hlim; // ~1 GB
  kernel_heap->free = 1;
  kernel_heap->prev = 0;
  kprintf("Memory init complete\n");
}

void *kmalloc(uint64_t size)
{
  //BREAK();
  if ((size == 0) || (size >= kernel_hlim))
    return 0;

  kmem_header *head = kernel_heap;
  while (((uint64_t)head < ((uint64_t)kernel_heap + kernel_hlim)) && 
    ((head->size < size) || !(head->free)))
  {
    //kprintf("MM: chunk %l, s %d - %d\n", 
    //  head, head->size, head->free);
    head = (kmem_header*)((uint64_t)head + head->size + sizeof(kmem_header));
  }

  // Если кончилась память
  if ((uint64_t)head >= ((uint64_t)kernel_heap + kernel_hlim))
    return 0;
  //BREAK();
  //kprintf("H: %l\n", head);
  // Если фрагмент можно и нужно разбить на два
  if (head->size > (size + sizeof(kmem_header)))
  {
    kmem_header *next = (kmem_header*)((uint64_t)head + size + sizeof(kmem_header));
    next->size = head->size - size - sizeof(kmem_header);
    next->free = 1;
    next->magic = KMEM_MAGIC;
    next->prev = head;
    // Коррекируем ссылку у бывшего последующим фрагмента
    if (((uint64_t)head + head->size) < 
    ((uint64_t)kernel_heap + kernel_hlim))
    {
      kmem_header *wnext = (kmem_header*)((uint64_t)head + head->size + sizeof(kmem_header));
      wnext -> prev = next;
    }

    head -> size = size;
  }
  head -> free = 0;
  return (void*)((uint64_t)head + sizeof(kmem_header));
}

void kfree(void *p)
{
  if (p == 0)
    return;
  /*
   * Основная идея:
   * Освобождаем блок, затем смотрим на последующий; если он
   * свободен, присоединяем его. Аналогично смотрим на предыдущий,
   * если он свободен - присоединяемся к нему.
   * Если размер получившегося в итоге блока больше размера страницы, 
   * то можно попробовать освободить физическую память; с другой 
   * стороны, ядро крайне редко использует kmalloc для выделения
   * памяти под временные буфера - большей частью они будут оставаться
   * на протяжении всего выполнения программы.
   */
  kmem_header *ptr = (kmem_header*)((uint64_t)p - sizeof(kmem_header));
  ptr->free = 1;
  kmem_header *prev = ptr->prev;
  if ((prev != 0) && (prev->magic == KMEM_MAGIC) && (prev->free == 1))
  {
    prev -> size += ptr->size + sizeof(kmem_header);
    ptr->magic = 0;
    ptr->size = 0;
    ptr->prev = 0;

    ptr = prev;
  }

  kmem_header *next = (kmem_header*)((uint64_t)ptr + ptr->size + sizeof(kmem_header));
  if ((next != 0) &&
      ((uint64_t)next < (uint64_t)kernel_heap + kernel_hlim) 
      && (next->magic == KMEM_MAGIC) && (next->free == 1))
  {
      ptr->size += next->size + sizeof(kmem_header);
      next->magic = 0;
      next->size = 0;
      next->prev = 0;
      next = (kmem_header*)((uint64_t)ptr + ptr->size + sizeof(kmem_header));
  }

  if ((next != 0) &&
      ((uint64_t)next < (uint64_t)kernel_heap + kernel_hlim) 
      && (next->magic == KMEM_MAGIC))
    // Обновляем указатель на предыдущий элемент
    next -> prev = ptr;

  //kprintf("Freed %X bytes\n", ptr->size);
  if (ptr->size >= 0x1000)
  {
    // Есть резон попробовать освободить физическую память
  }
}

