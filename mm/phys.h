#ifndef __PHYS_H__
#define __PHYS_H__

#include <multiboot.h>
#include <stdint.h>

void phys_init(uint64_t *pa, uint64_t ps, multiboot_info_t *m);

void *alloc_phys_page();
void free_phys_page(void *p);

#endif //__PHYS_H__
