#include <mem.h>

void mem_init(unsigned long pl_addr, unsigned long pl_size)
{
        unsigned long last_phys_page = (pl_addr&0x0FFFF000) + 0x1000;
        printf("Last phys page is %l\n", last_phys_page);
        page_init(&last_phys_page);
	phys_init(&last_phys_page);
}

void *kmalloc(unsigned long size)
{
}

void kfree(void *p)
{
}

