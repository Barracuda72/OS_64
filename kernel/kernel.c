#include <ktty.h>
#include <klibc.h>
#include <cpuid.h>
#include <gdt.h>
#include <task.h>
#include <intr.h>
#include <smp.h>
#include <phys.h>
#include <page.h>
#include <multiboot.h>
#include <mutex.h>

#include <debug.h>

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
    printf("Sorry, this kernel relies heavily on information,\n");
    printf("provided by Multiboot-compiliant bootloader!\n");
    printf("System halted.");
    return -1;
  }

  // Общая информация о памяти
  //printf ("mem_lower = %dKB, mem_upper = %dKB\n",
  //       (uint32_t) mb->mem_lower, (uint32_t) (mb->mem_upper));
  intr_init();

  printf("OS_64 build date: %s %s\n", __DATE__, __TIME__);

  smp_init();
  extern uint64_t kernel_end;
  uint64_t pool = &kernel_end;
  mem_init(pool, (mb->mem_upper>>10)+2, mb);  // TODO: Исправить!

  task_init();

  printf("Kernel alive, up and running!\n");

  char *test_text = "Syscall %d test\n";

  // Тест многозадачности
  if (task_fork() == 0)
  {
    // Дочерняя задача
    for(;;) 
    {
      //ktty_putc('C');
      // Тест системных вызовов
      syscall_test_out(test_text, 51);
      asm("hlt");
    }
  } else {
    for(;;) 
    {
      ktty_putc('P');
      asm("hlt");
    }
  }
  return 0;
}

