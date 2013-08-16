.org 0x7C00
.align 0

.text

.code16
// Код будет одним и тем же что для BSP, что для AP
// Только AP не будут выполнять всю инициализацию -
// просто выведут lapic ID и замрут.
// Кроме того, адресную линию A20 можно открыть только
// один раз. Это сделает BSP
.globl _start
_start:
  cli

  // Включим адресную линию A20
  inb $0x60, %al
  or $2, %al
  outb %al, $0x60
ap_start:
  cli
  cld
  // Переходим в 32-bit режим
  mov %cr0, %eax
  or $1, %eax
  mov %eax, %cr0
  lgdt gdtr
  ljmp $0x08, $code32

// "Трамплин" для AP
ap_jump:
  ljmp $0x00, $ap_start

.code32
code32:
  mov $0x10, %ax
  mov %ax, %es
  mov %ax, %ds

  mov $0xFEE00000, %ebx // EBX = APIC ID register's start
  // Ждем возможности вывести APIC ID
  mov $0x01, %al
.l0:
  xchg is_locked, %al
  and $0x1, %al
  jnz .l0

  // Считаем APIC ID
  mov 0x20(%ebx), %eax // 0xFEE00020 = APIC ID reg
  rol $8, %eax
  and $0xF, %al
  or $0x30, %al
  mov %al, apic_id

  // Выведем
  mov $text_apic_id, %esi
  mov vid_mem, %edi
  mov $9, %ecx // strlen
  mov $0x7, %ah // белый на черном, без миганий

.l1:
  movb (%esi), %al
  stosw
  inc %esi
  loop .l1
  
  // Сдвинем адрес отображения на новую строчку
  add $((80 - 9)*2), %edi
  mov %edi, vid_mem
  // Разблокируем mutex
  movb $0x00, is_locked

  mov $0x01, %al
  xchg bsp_on, %al
  and $0x1, %al
  jnz .lh // Мы - AP, значит, просто зависаем

  // Инициализируем AP
  // Скоприруем прыжок на наш код по адресу,
  // с которого AP начнет исполнение
  mov $ap_jump, %esi
  mov $0x8000, %edi
  mov $15, %ecx
  rep movsb

  // Установим таймер
  in  $0x61, %al // Считаем значение порта B
  and $0xFD, %al // Выключим PC-Speaker
  or  $0x01, %al // Включим второй канал
  out %al, $0x61 // Запишем новое значение

  // Установим канал 2 на единичное срабатывание
  mov $0xB0, %al // chn2, r/w LSB/MSB
  out %al, $0x43 // issue PIT command

  // Начнем!
  mov $0x000C4500, %eax // INIT-IPI всем, кроме себя
  mov %eax, 0x300(%ebx)
.l2:
  bt $12, 0x300(%ebx) // Ждем завершения команды
  jc .l2

  // Задержка в 10 миллисекунд, ждем пробуждения AP
  mov $0x10000, %eax
  call wait

  // Intel рекомендует повторять SIPI дважды с 
  // задержкой между повторами в 200 микросекунд
  mov $2, %ecx
.l4:
  // Последние 8 бит определяют вектор, с которого 
  // начнется выполнение.
  // 08<<12 = 0x8000
  mov $0x000C4608, %eax // Startup-IPI всем, кроме себя
  mov %eax, 0x300(%ebx)
.l3:
  bt  $12, 0x300(%ebx) // Ждем завершения команды
  jc  .l3

  // Задержка в 200 микросекунд между повторами
  mov $200, %eax
  call wait
  loop .l4

.lh:
  hlt
  jmp .lh

wait:
  // Получим делитель для таймера
  mov %eax, %ebp
  mov $100000, %eax  // Число микросекунд в секунде
  xor %edx, %edx
  div %ebp  // eax = (edx:eax)/%ebp

  mov %eax, %ebp
  mov $1193182, %eax // Частота таймера
  xor %edx, %edx
  div %ebp

  // ax = делитель для таймера
  out %al, $0x42
  inb $0x60, %al // Маленькая задержка
  mov %ah, %al
  out %al, $0x42

.l10:
  in      $0x61, %al
  test    $0x20, %al // Счетчик истек?
  jz      .l10
  ret

bsp_on:
.byte 0x00 // BSP уже запущен
is_locked:
.byte 0x00 // Вывод на экран заблокирован 
gdt:
.quad 0x0000000000000000
.quad 0x00CF9A000000FFFF // Код
.quad 0x00CF92000000FFFF // Данные
gdtr:
.short 0x23
.long  gdt  // Лимит и адрес
text_apic_id:
.ascii "APIC ID "
apic_id:
.byte 0x00
vid_mem:
.long 0xB8000

.org 0x7DFE
.word 0xAA55 // Сигнатура загрузочного флоппи
