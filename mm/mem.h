#ifndef __MEM_H__
#define __MEM_H__


void mem_init(unsigned long pl_addr, unsigned long pl_size);

void *kmalloc(unsigned long size);
void kfree(void *p);
#endif //__MEM_H__
