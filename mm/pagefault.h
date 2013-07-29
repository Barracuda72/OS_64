#ifndef __PAGEFAULT_H__
#define __PAGEFAULT_H__

#include <intr.h>
#include <klibc.h>

IRQ_HANDLER(page_fault)
{
        unsigned long addr,err,cs,eip;
	asm("pop %0 \n pop %1 \n pop %2":"=a"(err),"=b"(eip),"=c"(cs));
        asm("mov %%cr2, %0":"=a"(addr));
        printf("Page fault: %X\n",addr);
	printf("Flags: %b\n", err);
	printf("Caused by %X:%X\n", cs,eip);
        for(;;) asm("hlt");
}

#endif //__PAGEFAULT_H__
