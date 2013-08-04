#ifndef __PAGEFAULT_H__
#define __PAGEFAULT_H__

#include <intr.h>
#include <klibc.h>

#include <debug.h>
/*
IRQ_HANDLER(page_fault)
{
	BREAK();
        unsigned long addr,err,cs,rip;
	asm("pop %0 \n pop %1":"=a"(err),"=b"(rip));
        asm("mov %%cr2, %0":"=a"(addr));
        printf("Page fault: %l\n",addr);
	printf("Flags: %b\n", err);
	printf("Caused by %l\n", rip);
        for(;;) asm("hlt");
}
*/

void page_fault(void);	// IRQ handler
asm("page_fault: \n \
 push %rdi \n \
 push %rsi \n \
 push %rdx \n \
 mov 0x18(%rsp), %rdi \n \
 mov 0x20(%rsp), %rdx \n \
 mov %cr2, %rsi \n \
 call _page_fault \n \
 pop %rdx \n \
 pop %rsi \n \
 pop %rdi \n \
 sub $0x10, %rsp \n \
 movb $0x20, %al \n \
 outb %al, $0x20 \n \
 iretq \n \
");

void _page_fault(unsigned long errcode, unsigned long addr, unsigned long rip)
{
        printf("Page fault: %l\n",addr);
        printf("Flags: %b\n", errcode);
        printf("Caused by %l\n", rip);
        for(;;) asm("hlt");
}

#endif //__PAGEFAULT_H__
