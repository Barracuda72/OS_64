/*
 * Код трамплина для AP
 */
.org 0x8000
/*
 * Флаги критичны!
 * Иначе секция будет пропущена либо компоновщиком, либо загрузчиком
 */
.section .ap_text, "awx", @progbits
.code16

ap_start:
  // Запретим прерывания
  cli
  // Загрузим GDT
  xor %ax, %ax
  mov %ax, %ds
  lgdt ap_gdtr
  // Включим 32-bit режим
  mov %cr0, %eax
  or $1, %eax
  mov %eax, %cr0
  // Перейдем в него
  ljmp $0x08, $code32

.code32
code32:
  // Поправим сегментные регистры
  mov $0x10, %ax
  mov %ax, %es
  mov %ax, %ds
  mov %ax, %ss
/* Прыгаем на инициализацию long mode */
  ljmp $0x08, $long_enter

.align 16
ap_gdt:
.quad 0x0000000000000000
.quad 0x00CF9A000000FFFF // Код
.quad 0x00CF92000000FFFF // Данные
ap_gdtr:
.short 23
.long  ap_gdt // Лимит и адрес
.long  0

