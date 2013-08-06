#ifndef __PAGEFAULT_H__
#define __PAGEFAULT_H__

#include <intr.h>
#include <klibc.h>
#include <phys.h>
#include <page.h>

#include <debug.h>

#define PF_PRESENT (1<<0)
#define PF_WRITE   (1<<1)
#define PF_USER    (1<<2)

void page_fault(void);	// IRQ handler
asm("page_fault: \n \
 push %rax \n \
 push %rdi \n \
 push %rsi \n \
 push %rdx \n \
 mov 0x20(%rsp), %rdi \n \
 mov 0x28(%rsp), %rdx \n \
 mov %cr2, %rsi \n \
 call _page_fault \n \
 pop %rdx \n \
 pop %rsi \n \
 pop %rdi \n \
 movb $0x20, %al \n \
 outb %al, $0x20 \n \
 pop %rax \n \
 add $0x8, %rsp \n \
 iretq \n \
");

// Адрес структуры кучи ядра
extern unsigned long kernel_heap;

void _page_fault(unsigned char errcode, unsigned long addr, unsigned long rip)
{
	// Если произошла ошибка при обращении к несуществующей странице
	// внутри кучи ядра, выделим страницу
	if ((addr >= kernel_heap) && 
	   (!(errcode&PF_PRESENT) && !(errcode&PF_USER)))
	{
	  //printf("Kernel heap fault, recovering...\n");
	  mount_page(alloc_phys_page(), addr&0xFFFFFFFFFFFFF000L);
	  //BREAK();
	  return;
	}
        printf("PF: a = %l, f = %b, rip = %l\n", 
	  addr, errcode, rip);

        for(;;) asm("hlt");
}

#endif //__PAGEFAULT_H__
