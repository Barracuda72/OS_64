/*
 * Очень простой read-only драйвер ATA
 */

.text
.code64
/*
 * Считать сектора с устройства
 */
.globl ata_pio_read
// void ata_pio_read(void *ptr, unsigned long lba, unsigned char secnum)
ata_pio_read:
// RDI, RSI, RDX
  pushfq
  push %rax
  push %rbx
  push %rcx
  push %rdx
  push %rsi
  push %rdi

  xor %rax, %rax
  xor %rbx, %rbx
  xor %rcx, %rcx

  mov %rsi, %rax
  mov %rdx, %rcx

  and $0x0FFFFFFF, %rax
  mov %rax, %rbx     // Сохраним LBA в %rbx
  
  mov $0x01F6, %edx  // Порт, на который отправится номер диска и биты 24-27 LBA
  shr $24, %rax      // Сдвинем биты 24-27 в %al
  or  $0xE0, %al     // Установим бит 6 для режима LBA (0xE0 = 11100000b)
  out %al, %dx

  mov $0x01F2, %edx  // Порт, принимающий количество секторов
  mov %cl, %al       // Получим количество секторов из %cl
  out %al, %dx

  mov $0x01F3, %edx  // Порт, принимающий биты 0-7 LBA
  mov %rbx, %rax     // Берем LBA из %rbx
  out %al, %dx

  mov $0x01F4, %edx  // Порт, принимающий биты 8-15 LBA
  // mov %rbx, %rax     // Берем LBA из %rbx
  shr $8, %rax       // Помещаем биты 8-15 в %al
  out %al, %dx

  mov $0x01F5, %edx  // Порт, принимающий биты 16-23 LBA
  // mov %rbx, %rax     // Берем LBA из %rbx
  shr $8, %rax       // Помещаем биты 16-23 в %al
  out %al, %dx

  mov $0x01F7, %edx  // Порт, принимающий команды
  mov $0x20, %al     // Чтение с повтором при ошибке
  out %al, %dx

  xor %rbx, %rbx
  mov %cl, %bl   // Количество секторов

wait:
  mov $0x01F7, %edx  // Порт, принимающий команды
  in %dx, %al
  test $8, %al // Операция выполняется
  jz wait      // Ждем

  mov $0x01F0, %rdx // Порт данных
  mov $128, %rcx // 128 двойных слов = 1 сектор
  cld
  rep insl  // Считываем в (%rdi)
  dec %rbx
  jnz wait

  pop %rdi
  pop %rsi
  pop %rdx
  pop %rcx
  pop %rbx
  pop %rax
  popfq
  ret
