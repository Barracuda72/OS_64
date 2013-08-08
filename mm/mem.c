#include <mem.h>
#include <multiboot.h>

#include <debug.h>

kmem_header *kernel_heap;
unsigned long kernel_hlim;

void mem_init(unsigned long pl_addr, unsigned long pl_size, 
		multiboot_info_t *mb)
{
	// last_page - последняя страница как физической,
	// так и виртуальной памяти
        unsigned long last_page = (pl_addr&0x0FFFF000) + 0x1000;
        //printf("Last phys page is %l\n", last_page);
        page_init(&last_page);
	phys_init(&last_page, pl_size, mb);
	// Размер кучи ~1Gb
	kernel_hlim = 0x40000000 - last_page - sizeof(kmem_header);
	kernel_heap = (kmem_header *)(last_page|0xFFFFFFFFC0000000);
	kernel_heap->magic = KMEM_MAGIC;
	kernel_heap->size = kernel_hlim; // ~1 GB
	kernel_heap->free = 1;
	kernel_heap->prev = 0;
	printf("Memory init complete\n");
}

void *kmalloc(unsigned long size)
{
	//BREAK();
	if ((size == 0) || (size >= kernel_hlim))
	  return 0;

	kmem_header *head = kernel_heap;
	while ((head < ((unsigned long)kernel_heap + kernel_hlim)) && 
		((head->size < size) || !(head->free)))
	{
		printf("MM: chunk %l, s %d - %d\n", 
			head, head->size, head->free);
		head = (unsigned long)head + head->size + sizeof(kmem_header);
	}

	// Если кончилась память
	if (head >= ((unsigned long)kernel_heap + kernel_hlim))
	  return 0;

	// Если фрагмент можно и нужно разбить на два
	if (head->size > (size + sizeof(kmem_header)))
	{
	  kmem_header *next = (unsigned long)head + size + sizeof(kmem_header);
	  next->size = head->size - size - sizeof(kmem_header);
	  next->free = 1;
	  next->magic = KMEM_MAGIC;
	  next->prev = head;
	  // Коррекируем ссылку у бывшего последующим фрагмента
	  if (((unsigned long)head + head->size) < 
		((unsigned long)kernel_heap + kernel_hlim))
	  {
	    kmem_header *wnext = (unsigned long)head 
				+ head->size + sizeof(kmem_header);
	    wnext -> prev = next;
	  }

	  head -> size = size;
	}
	head -> free = 0;
	return (unsigned long)head + sizeof(kmem_header);
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
   kmem_header *ptr = (unsigned long)p - sizeof(kmem_header);
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

   kmem_header *next = (unsigned long)ptr + ptr->size + sizeof(kmem_header);
   if ((next != 0) &&
	((unsigned long)next < (unsigned long)kernel_heap + kernel_hlim) 
	&& (next->magic == KMEM_MAGIC) && (next->free == 1))
   {
       ptr->size += next->size + sizeof(kmem_header);
       next->magic = 0;
       next->size = 0;
       next->prev = 0;
       next = (unsigned long)ptr + ptr->size + sizeof(kmem_header);
   }

   if ((next != 0) &&
	((unsigned long)next < (unsigned long)kernel_heap + kernel_hlim) 
	&& (next->magic == KMEM_MAGIC))
     // Обновляем указатель на предыдущий элемент
     next -> prev = ptr;

   //printf("Freed %X bytes\n", ptr->size);
   if (ptr->size >= 0x1000)
   {
     // Есть резон попробовать освободить физическую память
   }
}

