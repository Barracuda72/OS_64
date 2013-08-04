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

#include <debug.h>

/* Главная функция */
long kernel_start(unsigned long mb_magic, multiboot_info_t *mb)
{
	GDT_init();
	task_init();
	ktty_init();

	if (mb_magic != MULTIBOOT_LOADER_MAGIC)
	{
	  printf("Sorry, this kernel relies heavily on information,\n");
	  printf("provided by Multiboot-compiliant bootloader!\n");
	  printf("System halted.");
	  return -1;
	}

        // Общая информация о памяти
        printf ("mem_lower = %dKB, mem_upper = %dKB\n",
               (unsigned int) mb->mem_lower, (unsigned int) (mb->mem_upper));
	intr_init();

	ktty_puts("Welcome to 64-bit OS written in C!\n");
	printf("OS build date %s %s\n", __DATE__, __TIME__);
/*	
	printf("CPU vendor: %s\n", CPU_get_vendor_string());
	printf("CPU Family: %i\n", CPU_get_info_family());
	printf("CPU Model: %i\n", CPU_get_info_model());
	printf("CPU Stepping: %i\n", CPU_get_info_stepping());
	printf("CPU Type: %i\n", CPU_get_info_type());
*/
	get_s();

/*
	ktty_puts("Pagefault emulation...\n");
	unsigned long *addr = 0xFFFFFFFFF0000000;
	addr[10] = 5;	// Fault!
*/
	smp_init();
	extern unsigned long kernel_end;
	unsigned long pool = &kernel_end;
	mem_init(pool, (mb->mem_upper>>10)+2, mb);	// TODO: Исправить!

	// Тест сразу подсистем физической и логической памяти
	unsigned char *new_video = 0x7ABEFAEDF000;	// Совершенно левый адрес
	mount_page(0xB8000, new_video);
	new_video[0] = 'Q';
	umount_page(new_video);
	for(;;) asm("hlt");
	return 0;
}

