#define __ASSEMBLY__
#include <multiboot.h>

.text

.code32

.globl  start

start:
	jmp     multiboot_entry

/* Выравниваем заголовок multiboot по границе 4 байт. */
.align  4
 
/* Заголовок Multiboot. */
multiboot_header:
	/* Сигнатура Multiboot */
	.long   MULTIBOOT_HEADER_MAGIC

	/* Флаги */
	.long   MULTIBOOT_HEADER_FLAGS

	/* Контрольная сумма */
 	.long   MULTIBOOT_HEADER_CHKSM

 	/* Так как мы используем ELF, то остальные поля 
 	 * определять нет необходимости.
 	 */

multiboot_entry:
cld
cli
/* Передаем в ядро 2 параметра из multiboot:
 * Адрес информационной структуры - ebx
 * Сигнатура multiboot - eax
 */
pushl %ebx
pushl %eax
/* 
   Отображаем первые два мегабайта один-в-один, так что мы можем использовать память, будто и нет никакой страничной адресации
   Кроме того, нужно отобразить ядро и в верхнюю память (по адресу 0xFFFFFFFFC0000000) 
*/

/* 
   Строим таблицы страниц
   Они будут выглядеть примерно так:

 Последние 4 бита:
 Бит 0 - Страница существует
 Бит 1 - Запись разрешена
 Бит 2 - Ring 3 имеет доступ к странице
 Бит 3 - Кеширование write through
 Бит 7 - Большая страница

 PML4 :
 dq 0x000000000000b00f = 00000000 00000000  00000000 00000000  00000000 00000000  10010000 00001111 
 times 511 dq 0x0000000000000000

 PDP : 
 dq 0x000000000000c00f = 00000000 00000000  00000000 00000000  00000000 00000000  10100000 00001111
 times 511 dq 0x0000000000000000

 PD : // Покрывает сразу 2 мегабайта -> PT не нужна (уже не покрывает, нужна)
 dq 0x000000000000d00f = 00000000 00000000  00000000 00000000  00000000 00000000  10100000 00001111
 times 511 dq 0x0000000000000000 

 PT :
 dq 0x000000000000010f = 00000000 00000000  00000000 00000000  00000000 00000000  00000000 00001111
 times 511 dq 0x0000000000000000 

 Для каждой таблицы установим:
 PML4 - 0 и 511
 PDP - 0 и 511
 PD - 0 и 0

 На самом деле, надо выделять отдельные PDP и PD для 511 и 510 элементов.
 Но так как в оригинальных таблицах эти записи не используются, можно их заюзать.
 В любом случае, это временно - до тех пор, пока страничная адресация не будет
 установлена окончательно
*/

xor %esi, %esi
xor %edi, %edi
xor %eax, %eax

/* 
   Базовый адрес для всех таблиц
   PML4 = 0xA000
   PDPT = 0xB000
   PDT  = 0xC000
   PT   = 0xD000
*/

mov $0xA000, %di

/* PML4 */
// 0 элемент
mov $0xb00f, %ax
stosw

xor %ax, %ax
mov $0x07FB, %cx
rep stosw

// 511 элемент
mov $0xb00f, %ax
stosw

xor %ax, %ax
mov $3, %cx
rep stosw

/* PDP */
// 0 элемент
mov $0xc00f, %ax
stosw

xor %ax, %ax
mov $0x07FB, %cx
rep stosw

// 511 элемент
mov $0xc00f, %ax
stosw

xor %ax, %ax
mov $3, %cx
rep stosw

/* PD */
// 0 элемент
mov $0xd00f, %ax
stosw

xor %ax, %ax
mov $0x07FF, %cx
rep stosw

/* PT */
/* Первые два мегабайта отображаются здесь */
mov $0x400, %cx
xor %eax, %eax
mov $0xf, %eax

loo1:
mov %eax, (%edi)
add $4, %edi
movl $0, (%edi)
add $4, %edi
add $0x1000, %eax
dec %cx
jnz loo1

/* Entering long mode... */
mov $0xA0, %eax	/* Set PAE and PGE */
mov %eax, %cr4
mov $0x0000a000, %edx

/* Point CR3 at PML4 */
mov %edx, %cr3
mov $0xC0000080, %ecx 	/* Specify EFER MSR */
rdmsr	

/* Enable long mode */
or $0x00000100, %eax
wrmsr
mov %cr0, %ebx

/* Activate long mode */
or $0x80000001, %ebx

/* by enabling paging and protection simultaneously */
mov %ebx, %cr0

//xchg %bx, %bx

lgdt gdtpointer	/* Load 80-bit gdt pointer below */
//ljmp $gdtcode, $startLongMode	/* Load CS with 64-bit segment and flush the instruction cache */
// Восстановим адрес multiboot-структуры и Magic
popl %eax
popl %ebx
ljmp $8, $startLongMode

.code64

startLongMode:
cli	/* Interrupts are disabled - no IDT set up */
/* Инициализируем стек. */
movq    $(stack + STACK_SIZE), %rsp
// Корректируем стек
or $0xFFFFFFFFC0000000, %rsp
// Передаем параметры от Multiboot-загрузчика
xor %rsi, %rsi
xor %rdi, %rdi
mov %eax, %edi
mov %ebx, %esi
/* Вызываем главную функцию. */
//xchg %bx, %bx
call    kernel_start

loop:
hlt
jmp loop	// Hang up

/* Область нашего временного стека. */
.comm   stack, STACK_SIZE

/* Global Descriptor table */
gdt:
.quad 0x0000000000000000	/* First descriptor filled with 0 */
//.equ gdtcode, $ - gdt
.quad 0x0020980000000000	/* Code */
//.equ gdtdata, $ - gdt
.quad 0x0000900000000000	/* Data */

gdtpointer:
//.short $ - gdt - 1	/* 16-bit Base Size (limit) */
.short 0xFFFF
.quad gdt	/* 64-bit Base address */

//.globl IDT_rsrvd
//.comm IDT_rsrvd, PAGE_SIZE

