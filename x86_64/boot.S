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
 * Сохраним их в регистрах esp и ebp, которые пока не 
 * используются - стека нет
 */
  movl %ebx, %esp
  movl %eax, %ebp
/*
 * Проверяем, поддерживает ли процессор long mode
 */
  mov $0x80000001, %eax
  cpuid
  and $0x20000000, %edx
  jz no_x86_64

/* 
 * Отображаем первые два мегабайта один-в-один, так что мы можем
 * использовать память, будто и нет никакой страничной адресации.
 * Кроме того, нужно отобразить ядро и в верхнюю память (по адресу 
 * 0xFFFFFFFFC0000000) 
 */

/* 

 Последние 8 бит страницы:
 Бит 0 - Страница существует
 Бит 1 - Запись разрешена
 Бит 2 - Ring 3 имеет доступ к странице
 Бит 3 - Кеширование write through
 Бит 7 - Большая страница

 Строим временные таблицы страниц

 Для каждой таблицы установим:
 PML4 - 0 и 511
 PDP - 0 и 511
 PD - 0 и 0

 На самом деле, надо выделять отдельные записи для 0 и 511 элементов.
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

/* Теперь BSP подготавливает код для AP */
  mov $ap_bootcode, %esi
  mov $ap_bootcode_end, %ecx
  sub %esi, %ecx
  shr $2, %ecx
  mov $0x8000, %edi
  rep movsl

/* На long_enter будет прыгать наш AP */
.globl long_enter
long_enter:
/* Входим в длинный режим... */
  mov $0xA0, %eax  /* Set PAE and PGE */
  mov %eax, %cr4
  mov $0x0000a000, %edx

/* Загружаем в CR3 адрес PML4 */
  mov %edx, %cr3
  mov $0xC0000080, %ecx   /* Указываем EFER MSR */
  rdmsr  

/* Включаем длинный режим */
  or $0x00000100, %eax
  wrmsr
  mov %cr0, %ebx

/* И входим в него */
  or $0x80000001, %ebx

/* одновременно включая защиту и страничную адресацию */
  mov %ebx, %cr0

  lgdt gdtpointer  /* Загружаем 80-битный указатель на GDT */
// Передаем ядру адрес multiboot-структуры и Magic
  movl %esp, %esi
  movl %ebp, %edi
// Сбрасываем кэш инструкций и помещаем в CS адрес 64-битного
// сегмента кода
  ljmp $8, $startLongMode

/*
 * Вызывается при недоступности 64-битного режима
 */
no_x86_64:
  mov $x86_64_err, %esi
  mov $0xB8000, %edi
  mov $52, %ecx // strlen(x86_64_err)
  mov $0x7, %ah // = white on black, no flash

loo3:
  movb (%esi), %al
  stosw
  inc %esi
  loop loo3

loo2:
  hlt
  jmp loo2

.code64

startLongMode:
/* Отключим прерывания - IDT не установлена */
  cli
/* Инициализируем стек. */
  movq    $(stack + STACK_SIZE), %rsp
// Корректируем стек
  or $0xFFFFFFFFC0000000, %rsp

  // Проверяем, является ли текущий процессор BSP
  mov $1, %al
  xchg bsp_on, %al
  and $0x1, %al
  // Если нет - уходим в ожидание
  jnz loop

/* Вызываем главную функцию ядра. */
  call    kernel_start

loop:
  hlt
  jmp loop  // Hang up

bsp_on:
.byte 0x00

x86_64_err:
.asciz "You processor are not x86_64 capable! System halted."

/* Область нашего временного стека. */
.comm   stack, STACK_SIZE

/* Временная Global Descriptor Table */
gdt:
.quad 0x0000000000000000  /* Первый дескриптор - нулевой */
.quad 0x0020980000000000  /* Code */
.quad 0x0000900000000000  /* Data */

gdtpointer:
.short 23 /* 16-bit Base Size (limit) */
.quad gdt  /* 64-bit Base address */

