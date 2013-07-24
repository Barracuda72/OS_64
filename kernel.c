#include <ktty.h>
#include <klibc.h>
#include <cpuid.h>
#include <gdt.h>
#include <task.h>
#include <intr.h>
#include <smp.h>
#include <phys.h>

#include <debug.h>

/* Главная функция */
long kernel_start()
{
	GDT_init();
	task_init();
	ktty_init();

	intr_init();

	ktty_puts("Welcome to 64-bit OS written in C!\n");
	printf("OS build date %s %s\n", __DATE__, __TIME__);
	//ktty_puts("Have a nice day!\n");
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
	mem_init(pool, 0);	// TODO: Исправить!
	
	for(;;) asm("hlt");
	return 0;
}

