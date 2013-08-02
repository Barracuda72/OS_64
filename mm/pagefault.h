#ifndef __PAGEFAULT_H__
#define __PAGEFAULT_H__

#include <intr.h>
#include <klibc.h>

IRQ_HANDLER(page_fault)
{
        unsigned long addr,err,cs,rip;
	asm("pop %0 \n pop %1":"=a"(err),"=b"(rip));
        asm("mov %%cr2, %0":"=a"(addr));
        printf("Page fault: %l\n",addr);
	printf("Flags: %b\n", err);
	printf("Caused by %l\n", rip);
        for(;;) asm("hlt");
}

#endif //__PAGEFAULT_H__
