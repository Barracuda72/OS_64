#include <ktty.h>
#include <kprintf.h>
#include <cpuid.h>
#include <gdt.h>
#include <task.h>
#include <intr.h>
#include <smp.h>
#include <phys.h>
#include <page.h>
#include <multiboot.h>
#include <mutex.h>
#include <apic.h>

#include <ata_pio.h>

#include <fs.h>

#include <mem.h>
#include <vesa/vesa.h>

#include <ioport.h>

#include <debug.h>

uint64_t kernel_stack_new = 0xFFFFFFFFC0080000;
long kernel_run(void);

/* 
 * Главная функция 
 */
long kernel_start(uint64_t mb_magic, multiboot_info_t *mb)
{
  GDT_init();
  tss_init();
  ktty_init();

  if (mb_magic != MULTIBOOT_LOADER_MAGIC)
  {
    kprintf("Sorry, this kernel relies heavily on information,\n");
    kprintf("provided by Multiboot-compiliant bootloader!\n");
    kprintf("System halted.");
    return -1;
  }

  // Общая информация о памяти
  //kprintf ("mem_lower = %dKB, mem_upper = %dKB\n",
  //       (uint32_t) mb->mem_lower, (uint32_t) (mb->mem_upper));
  intr_init();

  kprintf("OS_64 build date: %s %s\n", __DATE__, __TIME__);

  extern uint64_t kernel_end;
  uint64_t pool = (uint64_t)&kernel_end;
  mem_init(pool, (mb->mem_upper>>10)+2, mb);  // TODO: Исправить!

  // Поправим адрес структуры multiboot
  mb = (multiboot_info_t*)((uint64_t)mb | 0xFFFFFFFFC0000000);

  if (mb->flags8 & MULTIBOOT_INFO_VINFO)
    vesa_init((void*)(mb->vbe_mode_info | 0xFFFFFFFFC0000000));
 
  // Чтобы AP не испортил нам стек
  // change_stack();
  alloc_pages((void*)kernel_stack_new, STACK_SIZE);

  asm volatile("\
    mov %0, %%rsp\n\
    xor %%rbp, %%rbp\n\
    call kernel_run\n\
  "::"a"(kernel_stack_new+STACK_SIZE));

  return 0;
}

long kernel_run(void)
{
  serial_init();
  fs_init();
  //BREAK();
  task_init();
  smp_init();

  kprintf("Kernel alive, up and running!\n");

  //kprintf("APIC ID %d\n", apic_get_id());

  //char *test_text = "Syscall %d test\n";
  char *text_video = (char*)0xFFFFFFFFC00B8000;
  char c = 0;
  // Тест многозадачности
  if (task_fork() == 0)
  {
    // Дочерняя задача
    // Крутим циферку в верхнем левом углу
    for(;;) 
    {
      volatile uint8_t apic_id = apic_get_id();
      //ktty_putc('C');
      // Тест системных вызовов
      //syscall_test_out(test_text, 51);
      text_video[0] = apic_id + 0x30;
      text_video[1] = c++;
      asm("hlt");
    }
  } else {
    // Тест ATA
#if 0
    char *bootsect = kmalloc(512);
    ata_pio_read(bootsect, 0, 1);
    int i;
    for (i = 0; i < 512; i++)
      kprintf("%x", bootsect[i]);
    kfree(bootsect);
#endif
    extern void fs_test_main(void);
    fs_test_main();

    // Крутим циферку в верхнем правом углу
    for(;;) 
    {
      //ktty_putc('P');
      volatile uint8_t apic_id = apic_get_id();
      text_video[158] = apic_id + 0x30;
      text_video[159] = c++;
      asm("hlt");
    }
  }
  return 0;
}

/*
 * Отсюда начнет выполнение наш AP
 */
long kernel_ap_start(uint8_t apic_id)
{
  //for (;;);
  // У AP меньше работы, чем у BSP. Ему не нужно инициализировать состояние
  // машины полностью - достаточно "вытянуть" только самого себя
  GDT_init_ap();
  // Перемещено в apic.c
  tss_init_cpu(apic_id);

  intr_init_ap();

  page_init_ap();

  apic_init_ap();

  task_init_cpu(apic_id);
  // BREAK();
  asm("sti");
  kprintf("AP ID %d init complete\n", apic_id);

  cpu_nr++;

  for(;;)
  {
    int i;
    apic_id = apic_get_id();
    ktty_putc(apic_id|0x30);
    //BREAK();
    asm("hlt");
  }
}
