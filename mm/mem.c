#include <mem.h>
#include <multiboot.h>

kmem_header *kernel_heap;

void mem_init(unsigned long pl_addr, unsigned long pl_size, 
		multiboot_info_t *mb)
{
	// last_page - последняя страница как физической,
	// так и виртуальной памяти
        unsigned long last_page = (pl_addr&0x0FFFF000) + 0x1000;
        //printf("Last phys page is %l\n", last_page);
        page_init(&last_page);
	phys_init(&last_page, pl_size, mb);
	kernel_heap = (kmem_header *)(last_page|0xFFFFFFFFC0000000);
	kernel_heap->magic = KMEM_MAGIC;
	kernel_heap->size = 0x40000000 - last_page; // ~1 GB
	kernel_heap->free = 1;
	kernel_heap->prev = 0;
	printf("Memory init complete\n");
}

void *kmalloc(unsigned long size)
{
}

void kfree(void *p)
{
}

