.org 0x7C00
.align 0
.text
.code16
.globl _start
_start:
  cld
  cli
  // По адресу 0x1000 будет наш временный буфер
  xor %ax, %ax
  mov %ax, %es
  mov %ax, %ds
  mov $0x1000, %di
  
  movl $0x33454256, (%di) // "VBE3"
  mov $0x4F00, %ax
  int $0x10 // GetVBEInfo

  mov 0xE(%di), %si
  mov $0x2000, %di

  // Перебираем доступные видеорежимы
  sub $2, %si
.l1:
  add $2, %si
  mov (%si), %cx
  cmp $0xFFFF, %cx
  je .l3
  mov $0x4F01, %ax
  int $0x10 // GetVideoModeInfo
  // di+0  - attributes
  // di+16 - bytes per scanline
  // di+18 - xres
  // di+20 - yres
  // di+25 - bpp
  // di+27 - memory model
  // di+40 - linear fb addr
  mov (%di), %bx // Get attributes word
  and $0x90, %bx // Linear FB support?
  cmp $0x90, %bx // НЕ and!
  jne .l1
  mov 0x1B(%di), %bx // Mem model: 4 = packed pixel, 6 = direct color
  cmp $4, %bx
  je .l2
  cmp $6, %bx
  jne .l1

.l2:
  // Подберем разрешение 800x600x16
  mov 0x12(%di), %bx
  cmp $800, %bx
  jne .l1
  mov 0x14(%di), %bx
  cmp $600, %bx
  jne .l1
  mov 0x19(%di), %bl
  cmp $16, %bl
  jne .l1 // Нашли нужный режим
  
.l3:
  // Если режим найден
  cmp $0xFFFF, %cx
  je .l4
  // То установим его
  mov $0x4F02, %ax
  mov %cx, %bx
  //or $0x90, %bx // Включим использование фреймбуфера
  int $0x10 // SetVideoMode

.l4:
  // Включим адресную линию A20
  inb $0x60, %al
  or $2, %al
  outb %al, $0x60
  // Переходим в 32-bit режим
  mov %cr0, %eax
  or $1, %eax
  mov %eax, %cr0
  lgdt gdtr
  ljmp $0x08, $code32

.code32
code32:
  mov $0x10, %ax
  mov %ax, %es
  mov %ax, %ds
  cmp $0xFFFF, %cx
  je .lh
  // Мы находимся в VESA-режиме 800x600x16
  // Надо что-нибудь нарисовать
  mov 0x28(%di), %edi // Адрес фреймбуфера
  // Рисовать будем градиент вертикальными линиями
  // Цвет = X*81, это даст 64800 цветов из 65536 возможных
  //xchg %bx, %bx
  mov $81, %si
  xor %bx, %bx 
  // Цикл по строкам
.ly:
  xor %cx, %cx 
  // Цикл по столбцам
.lx: 
  mov %cx, %ax
  mul %si // ax = ax*si
  mov %ax, (%edi)
  add $2, %edi

  inc %cx
  cmp $800, %cx
  jne .lx
  inc %bx
  cmp $600, %bx
  jne .ly
 // Начало новой строки
  
.lh:
  hlt
  jmp .lh

gdt:
.quad 0x0000000000000000
.quad 0x00CF9A000000FFFF // Код
.quad 0x00CF92000000FFFF // Данные
gdtr:
.short 0x23, gdt // Лимит и адрес

.org 0x7DFE
.word 0xAA55 // Сигнатура загрузочного флоппи
