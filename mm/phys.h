#ifndef __PHYS_H__
#define __PHYS_H__

void phys_init(unsigned long *pa, unsigned long ps);

void *alloc_phys_page();
void free_phys_page(void *p);

#endif //__PHYS_H__
