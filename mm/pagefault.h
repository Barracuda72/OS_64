#ifndef __PAGEFAULT_H__
#define __PAGEFAULT_H__

#include <intr.h>
#include <kprintf.h>
#include <phys.h>
#include <page.h>
#include <stdint.h>
#include <regs.h>

#include <debug.h>

#define PF_PRESENT (1<<0)
#define PF_WRITE   (1<<1)
#define PF_USER    (1<<2)

void page_fault(void);  // IRQ handler
asm("page_fault: \n \
  call save_regs \n \
  mov %cr2, %rdi # Адрес ошибки, арг #0\n \
  mov %rsp, %rsi # Регитры места ошибки, арг #1\n \
  call _page_fault \n \
  call rest_regs \n \
  add $0x8, %rsp # Выкинем из стека флаги \n \
  iretq \n \
");

// Адрес структуры кучи ядра
extern uint64_t kernel_heap;

void _page_fault(uint64_t addr, all_regs *r)
{
  // Если произошла ошибка при обращении к несуществующей странице
  // внутри кучи ядра, выделим страницу
  if ((addr >= kernel_heap) && 
      (!(r->reg1&PF_PRESENT) && !(r->reg1&PF_USER)))
  {
    //kprintf("Kernel heap fault, recovering...\n");
    mount_page(alloc_phys_page(), (void*)(addr&0xFFFFFFFFFFFFF000L));
    //BREAK();
    return;
  }
  
  kprintf("Page Fault: address = %l, flags = %b\n", 
         addr, r->reg1);
  kprintf(
         "RAX %l | RBX %l | RCX %l\n"
         "RDX %l | RSI %l | RDI %l\n"
         "RBP %l | RSP %l | RIP %l\n"
         "RFL %l | R8  %l | R9  %l\n"
         ,
         r->c.rax, r->c.rbx, r->c.rcx, 
         r->c.rdx, r->c.rsi, r->c.rdi,
         r->c.rbp, r->i.rsp, r->i.rip, 
         r->i.rflags, r->c.r8,  r->c.r9
  );

  kprintf("Stack unwind:\n");
  uint64_t *stacktop = (uint64_t*)(r->i.rsp);
  int32_t i;
  for(i = -5; i < 5; i++)
    kprintf("%c0x%x: 0x%l\n", SIGN(i), ABS(i)<<3, stacktop[i]);

  BREAK();
  for(;;) asm("hlt");
}

#endif //__PAGEFAULT_H__
